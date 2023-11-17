/*
 * monitor_temperatura.c
 *
 *  Created on: 15 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#include "monitor_temperatura.h"
#include "driver/i2c.h"
#include "si7021.h"
#include "esp_log.h"

#define MUESTREO	CONFIG_MUESTREO

static const char *TAG = "Monitor_temperatura";

ESP_EVENT_DEFINE_BASE(MONITOR_TEMPERATURA_EVENTS);

static float TEMP, HUM;

static void read_values_timer_callback(void* arg){
	readTemperature(0, &TEMP);
	readHumidity(0, &HUM);
	esp_event_post(MONITOR_TEMPERATURA_EVENTS, READ_EVENT, NULL, 0, 100);
}

void monitor_temperatura_init(){
	esp_timer_handle_t read_timer;

    const esp_timer_create_args_t read_timer_args = {
            .callback = &read_values_timer_callback,
            .name = "read_temperature"
    };

    ESP_ERROR_CHECK(esp_timer_create(&read_timer_args, &read_timer));

    ESP_LOGI(TAG,"Iniciando un muestreo en cada %d seg.", MUESTREO);

    ESP_ERROR_CHECK(esp_timer_start_periodic(read_timer, MUESTREO * 1000000));
}

void monitor_temperatura_getSensorValues(float *temperature, float *humidity){
	*temperature = TEMP;
	*humidity = HUM;
}


