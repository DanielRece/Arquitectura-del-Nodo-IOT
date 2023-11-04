/*
 * main.h
 *
 *  Created on: 17 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#ifndef MAIN_APP_EVENTS_H_
#define MAIN_APP_EVENTS_H_


#include "esp_event.h"
//#include "main.c"

ESP_EVENT_DECLARE_BASE(APP_EVENTS);


typedef enum{
	GPIO_LEIDO,
	COMANDO_RECIBIDO,
	IP_CONECTADO,
	IP_PERDIDA
}eventos_t;


#endif /* MAIN_APP_EVENTS_H_ */
