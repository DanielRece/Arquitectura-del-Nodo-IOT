/*
 * monitor_temperatura.h
 *
 *  Created on: 15 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#ifndef COMPONENTS_MONITOR_TEMPERATURA_INCLUDES_MONITOR_TEMPERATURA_H_
#define COMPONENTS_MONITOR_TEMPERATURA_INCLUDES_MONITOR_TEMPERATURA_H_

#include "esp_event.h"
#include "esp_timer.h"

ESP_EVENT_DECLARE_BASE(MONITOR_TEMPERATURA_EVENTS);


typedef enum{
	READ_EVENT
}monitor_temperatura_eventos_t;

void monitor_temperatura_init();
void monitor_temperatura_getSensorValues(float *temperature, float *humidity);



#endif /* COMPONENTS_MONITOR_TEMPERATURA_INCLUDES_MONITOR_TEMPERATURA_H_ */
