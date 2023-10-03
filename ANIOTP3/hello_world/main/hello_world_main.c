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

static float temp;

static void temp_task_function(void *arg)
{
    TickType_t period = arg;
    while(1){
        temp = rand() + rand()/RAND_MAX;
        ESP_LOGI("Muestra", "Value:%f",temp);
        vTaskDelay(period);
    }    
}

void app_main(void)
{
    printf("Hello world<!\n");
    TickType_t period = 1000/portTICK_PERIOD_MS;
    xTaskCreatePinnedToCore(&temp_task_function, "Muestreadora", 2048, &period, 3, NULL, 0);
    printf("Bye world");
}
