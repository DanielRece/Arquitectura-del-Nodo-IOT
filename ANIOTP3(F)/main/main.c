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

#include "app_events.h"
#include "mock_wifi.h"

#define FLASH_SIZE	1024

ESP_EVENT_DEFINE_BASE(APP_EVENTS);


typedef enum estados_e{
	EstadoConsola,
	EstadoMonitorSC,
	EstadoMonitorCC
}estados_e;


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
	esp_event_post(APP_EVENTS, GPIO_LEIDO, NULL, 0, 100);
}

void app_main(void)
{
	estadoActual = EstadoMonitorSC;

	i2c_master_init();
	initFlash(FLASH_SIZE);

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_event_handler_instance_register(APP_EVENTS, ESP_EVENT_ANY_ID, app_events_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(MOCK_WIFI_EVENTS, ESP_EVENT_ANY_ID, mock_wifi_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(CONSOLA_EVENTS, CONSOLA_EVENTS_MONITORIZACION, console_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(MONITOR_TEMPERATURA_EVENTS, READ_EVENT, monitor_temperatura_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(MONITOR_GPIO_EVENTS, READ_GPIO_EVENT, monitor_gpio_handler, NULL, NULL));

	monitor_temperatura_init();
	monitor_gpio_init();
	consolaEvent_init();
	wifi_connect();

	vTaskDelete(NULL);
}
