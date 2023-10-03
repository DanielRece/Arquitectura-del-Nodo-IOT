#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "i2c_config.h"
#include "esp_log.h"
#include "si7021.h"

#include "esp_timer.h"
#include "nvs.h"


const static gpio_num_t led_gpio[4] = {GPIO_NUM_12,GPIO_NUM_13,GPIO_NUM_14,GPIO_NUM_15};

static const char *TAG = "main";

static float temperature;

static void configure_leds(void)
{
	for (int i = 0; i < 4; i++){
		gpio_reset_pin(led_gpio[i]);
		gpio_set_direction(led_gpio[i], GPIO_MODE_OUTPUT);
	}
}

static void read_temperature_timer_callback(void* arg){
	float aux;
	int i;

	readTemperature(0, &temperature);

	aux = temperature;
	i = 0;

	aux -= 20.0;
	while(aux >= 0 && i < 4 ){
		gpio_set_level(led_gpio[i], 1);
		aux -= 2.0;
		i++;
	}
	ESP_LOGW(TAG,"SE HAN ENCENDIDO %d LEDS", i);
	for (;i<4;i++){
		gpio_set_level(led_gpio[i], 0);
	}


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

    configure_leds();


    ESP_ERROR_CHECK(esp_timer_create(&read_timer_args, &read_timer));
    ESP_ERROR_CHECK(esp_timer_create(&print_timer_args, &print_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(read_timer, 1000000));
    ESP_ERROR_CHECK(esp_timer_start_periodic(print_timer, 10000000));
}

