#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "i2c_config.h"
#include "esp_log.h"
#include "si7021.h"

#include "esp_timer.h"

//static const char *TAG = "main";

static int BLINK_GPIOS[4] = {5,18,21,22};
static uint8_t s_led_states[4] = {0,0,0,0};

static float temperature;

static void read_temperature_timer_callback(void* arg){
	//ESP_LOGI("READ_TIMER","Reading...");
	readTemperature(0, &temperature);
	float cp_temp = temperature;
        int i = 0;
        while (cp_temp > 0 && i < 4){
                s_led_states[i] = 1;
                cp_temp -=2;
                i++;
        }
        while (i < 4){
                s_led_states[i]=0;
                i++;
        }
        gpio_set_level(BLINK_GPIOS[0], s_led_states[0]);
        gpio_set_level(BLINK_GPIOS[1], s_led_states[1]);
        gpio_set_level(BLINK_GPIOS[2], s_led_states[2]);
        gpio_set_level(BLINK_GPIOS[3], s_led_states[3]);
        
        
        // TODO: Encender/apagar LEDs en función de la temperatura
	/*
		EJEMPLO DE PSEUDOCODIGO DE CÓMO HACER LA LÓGICA
  (HABRÍA QUE INICIALIZAR LOS GPIOS EN EL MAIN)
  temp -= 20;
i = 0;
while (temp > 0 && i < 4){
 encender_led(gpio[i]);
 temp -= 2;
 i++;
}
// APAGAR LOS LEDS RESTANTES
while(i < 4){
apagar_led(gpio[i]);
i++;
}

	*/

}
static void print_temperature_timer_callback(void* arg){
	ESP_LOGI("PRINT_TIMER","Temperature: %f", temperature);
}

void app_main(void)
{
        esp_timer_handle_t read_timer, print_timer;

        const esp_timer_create_args_t read_timer_args = {
                .callback = &read_temperature_timer_callback,
                .name = "read_temperature"
        };

        const esp_timer_create_args_t print_timer_args = {
                .callback = &print_temperature_timer_callback,
                .name = "print_temperature"
        };

        nvs_flash_init();

        esp_timer_init();

        i2c_master_init();


        ESP_ERROR_CHECK(esp_timer_create(&read_timer_args, &read_timer));

        ESP_ERROR_CHECK(esp_timer_create(&print_timer_args, &print_timer));
        
        ESP_ERROR_CHECK(esp_timer_start_periodic(read_timer, 1000000));

        ESP_ERROR_CHECK(esp_timer_start_periodic(print_timer, 10000000));
}

