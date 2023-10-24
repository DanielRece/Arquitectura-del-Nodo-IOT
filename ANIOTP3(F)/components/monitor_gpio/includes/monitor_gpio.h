/*
 * monitor_gpio.h
 *
 *  Created on: 20 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#ifndef COMPONENTS_MONITOR_GPIO_INCLUDES_MONITOR_GPIO_H_
#define COMPONENTS_MONITOR_GPIO_INCLUDES_MONITOR_GPIO_H_

#include "freertos/FreeRTOS.h"
#include <esp_event.h>
#include <driver/gpio.h>
#include <esp_log.h>

ESP_EVENT_DECLARE_BASE(MONITOR_GPIO_EVENTS);


typedef enum{
	READ_GPIO_EVENT
}monitor_gpio_eventos_t;

void monitor_gpio_init();



#endif /* COMPONENTS_MONITOR_GPIO_INCLUDES_MONITOR_GPIO_H_ */
