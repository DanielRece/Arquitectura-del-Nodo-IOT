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

static float temp;

static void temp_task_function(void *arg)
{
    TickType_t period = (TickType_t) arg;
    while(1){
        temp = rand() + rand()/RAND_MAX;
        ESP_LOGI("Muestra", "Value:%f",temp);
        vTaskDelay(period);
    }    
}

void app_main(void)
{
    srand(time(NULL));
    ESP_LOGI("INFO", "Hello world<!\n");
    TickType_t period = 1000/portTICK_PERIOD_MS;
    xTaskCreatePinnedToCore(&temp_task_function, "Muestreadora", 2048, (void*) period, 2, NULL, 0);
    ESP_LOGI("INFO", "Bye world");
}
