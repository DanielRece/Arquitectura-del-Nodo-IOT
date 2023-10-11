/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_system.h"
#include <driver/adc.h>
#include "esp_log.h"
#include "time.h"
#include "../components/mock-flash/mock-flash.h"

static CircularBuffer buffer;

void lee_estados(){
    switch (DATOS_SENSOR_OBTENIDOS){
        datos_sensor = obtener_sensor();
        case (WIFI_CONECTADO):
            while(getDataLeft(&buffer)>0){
                float dato_a_enviar = (*float)*readFromFlash(&buffer, sizeof(float));
                enviar_por_wifi(dato_a_enviar);
            }
            enviar_por_wifi(datos_sensor)
        case (WIFI_NO CONECTADO):
            writeToFlash($buffer, datos_sensor, sizeof(datos_sensor));
    }
}





void app_main() {




}