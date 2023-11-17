#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include <esp_err.h>
#include <esp_log.h>
#include "i2c_config.h"
#include <string.h>



#include "mock_flash.h"
#include "consola.h"
#include "monitor_temperatura.h"
#include "monitor_gpio.h"
#include "ota_manager.h"

#include "app_events.h"
#include "mock_wifi.h"


#include "esp_pm.h"
#include "esp_timer.h"
#include "sleep_config.h"
#include "esp_sleep.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "nvs.h"

#define FLASH_SIZE	1024

ESP_EVENT_DEFINE_BASE(APP_EVENTS);

static esp_timer_handle_t myTimer;

typedef enum estados_e{
	EstadoConsola,
	EstadoMonitorSC,
	EstadoMonitorCC
}estados_e;

float ultima_temperatura = 0.0;
float ultima_humedad = 0.0;

static estados_e estadoActual;

static const char TAG[] = "MAIN";


void task_envio(void * pvParameters){
	static float *temp, *hum;
	char data[32];
	ESP_LOGI(TAG,"INICIANDO ENVIO DE DATOS PENDIENTES (%d BYTES)", getDataLeft());
	while(getDataLeft() > 0 && estadoActual == EstadoMonitorCC){
		hum = (float*)readFromFlash(sizeof(float));
		snprintf(data, 32, "%.2f", *hum);
		send_data_wifi(data, strlen(data));
		free(hum);

		temp = (float*)readFromFlash(sizeof(float));
		snprintf(data, 32, "%.2f", *temp);
		send_data_wifi(data, strlen(data));
		free(temp);

	}
	vTaskDelete(NULL);
}


static void app_events_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
	eventos_t evento = (eventos_t) event_id;

		switch(estadoActual){
			case EstadoConsola:
				if (evento == COMANDO_RECIBIDO) {
					esp_log_level_set(TAG, ESP_LOG_INFO);
					ESP_LOGI(TAG,"PASANDO DE EstadoConsola A EstadoMonitorSC");
					estadoActual = EstadoMonitorSC;
					wifi_connect();
				}
				break;

			case EstadoMonitorSC:
				if (evento == IP_CONECTADO) {
					ESP_LOGI(TAG,"PASANDO DE EstadoMonitorSC A EstadoMonitorCC");
					estadoActual = EstadoMonitorCC;
					if (getDataLeft() > 0) xTaskCreate(task_envio,"task_envio",2048, NULL, 5, NULL);
				}else if (evento == GPIO_LEIDO) {
					ESP_LOGI(TAG,"PASANDO DE EstadoMonitorSC A EstadoConsola");
					estadoActual = EstadoConsola;
					esp_log_level_set(TAG, ESP_LOG_NONE);
					wifi_disconnect();
					consola_init();
				}
				break;

			case EstadoMonitorCC:
				if(evento == GPIO_LEIDO) {
					ESP_LOGI(TAG,"PASANDO DE EstadoMonitorCC A EstadoConsola");
					estadoActual = EstadoConsola;
					esp_log_level_set(TAG, ESP_LOG_NONE);
					wifi_disconnect();
					consola_init();
				}
				else if (evento == IP_PERDIDA) {
					ESP_LOGI(TAG,"PASANDO DE EstadoMonitorCC A EstadoMonitorSC");
					estadoActual = EstadoMonitorSC;
					wifi_connect();
				}
				else if (evento == GPIO_OTA_LEIDO){
					ESP_LOGI(TAG,"Iniciando OTA update...");
					ota_manager_update();
				}
				break;

			default:
				estadoActual = EstadoMonitorSC;
				break;
		}
}


static void mock_wifi_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
	switch((wifi_events_t)event_id){
		case (WIFI_CONECTADO):
			ESP_LOGI(TAG,"Wifi Conectado");
			break;

		case (WIFI_DESCONECTADO):
			ESP_LOGI(TAG,"Wifi Desconectado");
			esp_event_post(APP_EVENTS, IP_PERDIDA, NULL, 0, 100);
			break;

		case (WIFI_IP_CONSEGUIDA):
			ESP_LOGI(TAG,"IP Conseguida");
			esp_event_post(APP_EVENTS, IP_CONECTADO, NULL, 0, 100);
			break;
	}
}


void console_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
	ESP_LOGI(TAG,"Recibido CONSOLA_EVENTS_MONITORIZACION");
	esp_event_post(APP_EVENTS, COMANDO_RECIBIDO, NULL, 0, 100);
}

void monitor_temperatura_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
	float temp, hum;
	char data[32];
	ESP_LOGI(TAG,"RECIBIDO EVENTO DE MONITOR DE TEMPERATURA");

	monitor_temperatura_getSensorValues(&temp, &hum);

	ultima_temperatura = temp;
	ultima_humedad = hum;

	switch(estadoActual){
		case EstadoMonitorSC:
			if (getDataLeft() + (sizeof(float)*2) <= FLASH_SIZE ){
				writeToFlash(&temp, sizeof(float));
				writeToFlash(&hum, sizeof(float));
			}
			ESP_LOGI(TAG,"Quedan %d BYTES en la FLASH", FLASH_SIZE - getDataLeft());

			break;
		case EstadoMonitorCC:
			snprintf(data, 32, "%.2f", temp);
			send_data_wifi(data, strlen(data));

			snprintf(data, 32, "%.2f", hum);
			send_data_wifi(data, strlen(data));
			break;
		default:
			break;
	}
}

void monitor_gpio_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
	ESP_LOGI(TAG,"GPIO_LEIDO");
	switch ((monitor_gpio_eventos_t) event_id){
	case READ_GPIO_EVENT:
		ESP_LOGI(TAG,"Leído GPIO");
		esp_event_post(APP_EVENTS, GPIO_LEIDO, NULL, 0, 100);
		break;

	case READ_GPIO_OTA_EVENT:
		ESP_LOGI(TAG,"Leído OTA GPIO");
		esp_event_post(APP_EVENTS, GPIO_OTA_LEIDO, NULL, 0, 100);
		break;

	}

}

static void timer_callback (void*){
	ESP_LOGI("TIMER", "Entering deep sleep due timer 12h");
	//example_register_gpio_wakeup();
	example_register_sleep_timer_wakeup();
	char str_ultima_temperatura[20];
	char str_ultima_humedad[20];

	snprintf(str_ultima_temperatura, sizeof(str_ultima_temperatura), "%f", ultima_temperatura);
	snprintf(str_ultima_humedad, sizeof(str_ultima_humedad), "%f", ultima_humedad);
	
	nvs_handle_t nvs_handle;
	ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
	ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "temp", str_ultima_temperatura));
	ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "hum", str_ultima_humedad));
	ESP_ERROR_CHECK(nvs_commit(nvs_handle));
	nvs_close(nvs_handle);
	uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
	esp_deep_sleep_start();
}


static void nvs_wakeup_reason(void){
	nvs_handle_t nvs_handle;
	ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));

	const char* wakeup_reason;
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_TIMER:
			wakeup_reason = "timer";
			break;
		case ESP_SLEEP_WAKEUP_GPIO:
			wakeup_reason = "pin";
			break;
		default:
			wakeup_reason = "other";
			break;
	}
	#if CONFIG_NEWLIB_NANO_FORMAT
			/* printf in newlib-nano does not support %ll format, causing example test fail */
			printf("Returned from sleep, reason: %s\n", wakeup_reason);
		#else
			printf("Returned from sleep, reason: %s\n", wakeup_reason);
		#endif
	
	if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
		/* Waiting for the gpio inactive, or the chip will continously trigger wakeup*/
		example_wait_gpio_inactive();
	}

	nvs_set_str(nvs_handle, "wr", wakeup_reason);
	ESP_ERROR_CHECK(nvs_commit(nvs_handle));
	nvs_close(nvs_handle);
}


static void nvs_print_and_erase(void){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));

    char str_ultima_temp[20];
    char str_ultima_hum[20];
    char wakeup_reason[20];
	size_t sut = sizeof(str_ultima_temp);
	size_t suh = sizeof(str_ultima_hum);
	size_t swr = sizeof(wakeup_reason);

    err = nvs_get_str(nvs_handle, "temp", str_ultima_temp, &sut);
    if (err == ESP_OK){
        float ultima_temp = strtof(str_ultima_temp, NULL);
        ESP_LOGI("NVS", "La última temperatura tomada por el sensor antes del r	einicio es: %f", ultima_temp);
        ESP_ERROR_CHECK(nvs_erase_key(nvs_handle, "temp"));    
    }

    err = nvs_get_str(nvs_handle, "hum", str_ultima_hum, &suh);
    if (err == ESP_OK){
        float ultima_hum = strtof(str_ultima_hum, NULL);
        ESP_LOGI("NVS", "La última humedad tomada por el sensor antes del reinicio es: %f", ultima_hum);
        ESP_ERROR_CHECK(nvs_erase_key(nvs_handle, "hum"));
    }

    err = nvs_get_str(nvs_handle, "wr", wakeup_reason, &swr);
    if (err == ESP_OK){
        ESP_LOGI("NVS", "El motivo por el que se sale de deep_sleep es:%s", wakeup_reason);
        ESP_ERROR_CHECK(nvs_erase_key(nvs_handle, "wr"));
    }

    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

void app_main(void)
{
	ESP_LOGI(TAG,"VERSION OTA 1");
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ota_manager_partition_diagnosis();

	nvs_wakeup_reason();

	esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "myTimer_args",
        .skip_unhandled_events = true //sin este argumento no se comporta bien, acumula logs y los lanza todos de golpe al salir del sleep, también puede sacar del sleep sin necesidad de pulsar el gpio ni que pase el tiempo necesario.
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &myTimer));

    //esp_timer_start_once(myTimer, 60 * 1000 * 1000);  // Iniciar temporizador de alta resolución

    esp_pm_config_esp32_t config = {
		.light_sleep_enable = true,
	};
	esp_pm_configure(&config);
	
	nvs_print_and_erase();

	estadoActual = EstadoMonitorSC;

	i2c_master_init();
	initFlash(FLASH_SIZE);

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_event_handler_instance_register(APP_EVENTS, ESP_EVENT_ANY_ID, app_events_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(MOCK_WIFI_EVENTS, ESP_EVENT_ANY_ID, mock_wifi_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(CONSOLA_EVENTS, CONSOLA_EVENTS_MONITORIZACION, console_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(MONITOR_TEMPERATURA_EVENTS, READ_EVENT, monitor_temperatura_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(MONITOR_GPIO_EVENTS, ESP_EVENT_ANY_ID, monitor_gpio_handler, NULL, NULL));

	monitor_temperatura_init();
	monitor_gpio_init();
	consolaEvent_init();
	wifi_init();
	wifi_connect();

	vTaskDelete(NULL);
}
