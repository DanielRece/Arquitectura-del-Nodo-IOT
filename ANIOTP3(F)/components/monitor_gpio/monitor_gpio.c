/*
 * monitor_gpio.c
 *
 *  Created on: 20 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#include "monitor_gpio.h"

#define MUESTREO	CONFIG_DETECCION
#define GPIO_BUTTON	CONFIG_GPIO_BOTON

ESP_EVENT_DEFINE_BASE(MONITOR_GPIO_EVENTS);

static void task_lee_gpio(void *args){
	while(1){
		vTaskDelay((MUESTREO * 1000)/portTICK_PERIOD_MS);
		//ESP_LOGI("MONITOR_GPIO","LEVEL: %d", gpio_get_level(GPIO_BUTTON));

		if(gpio_get_level(GPIO_BUTTON)){
			vTaskDelay(50);
			if(gpio_get_level(GPIO_BUTTON)) esp_event_post(MONITOR_GPIO_EVENTS,READ_GPIO_EVENT,NULL,0,100);
		}


	}
	vTaskDelete(NULL);
}

void monitor_gpio_init(){
	ESP_ERROR_CHECK(gpio_reset_pin(GPIO_BUTTON));
	ESP_ERROR_CHECK(gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT));
	ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_BUTTON));
	xTaskCreate(task_lee_gpio,"task_lee_gpio",2048, NULL, 5, NULL);
}


