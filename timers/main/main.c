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

static const char *TAG = "main";

void app_main(void)
{
	struct si7021_reading si_data;
	i2c_port_t i2c_num;

    nvs_flash_init();

    i2c_master_init();

    i2c_num = 0;

     while (1) {
    	 readSensors(i2c_num, &si_data);

    	 ESP_LOGI(TAG, "-----------------------------");
         ESP_LOGI(TAG, "Reading SI7021:");
         ESP_LOGI(TAG,  "	Temperature: %f ", si_data.temperature);
         ESP_LOGI(TAG,  "	Humidity: %f ", si_data.humidity);
         vTaskDelay(2000 / portTICK_PERIOD_MS);
     }
}

