/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "light_sleep_example.h"

static esp_timer_handle_t myTimer;
static int lightSleepCount = 0;

static void timer_callback (void*){
    ESP_LOGI("TIMER", "Tiempo actual: %lld\n", esp_timer_get_time());
}


static void sleep_task(void *args)
{
    while (true){
        vTaskDelay(pdMS_TO_TICKS(3000));
        /* Get timestamp before entering sleep */
        int64_t t_before_us = esp_timer_get_time();

        if (lightSleepCount < 5) {
            lightSleepCount++;

            /* Enable wakeup from light sleep by timer */
            example_register_light_sleep_timer_wakeup();

            printf("Entering light sleep\n");
            /* To make sure the complete line is printed before entering sleep mode,
            * need to wait until UART TX FIFO is empty:
            */
            uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);

            /* Enter sleep mode */
            esp_light_sleep_start();

        } else {
            /* Enable wakeup from light sleep by timer */
            example_register_deep_sleep_timer_wakeup();

            printf("Entering deep sleep\n");
            /* To make sure the complete line is printed before entering sleep mode,
            * need to wait until UART TX FIFO is empty:
            */
            uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);

            /* Enter sleep mode */
            esp_deep_sleep_start();
        }
         /* Get timestamp after waking up from sleep */
        int64_t t_after_us = esp_timer_get_time();
        /* Determine wake up reason */
        const char* wakeup_reason;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                break;
            case ESP_SLEEP_WAKEUP_GPIO:
                wakeup_reason = "pin";
                break;
            default:
                wakeup_reason = "other";
                break;
        }
        #if CONFIG_NEWLIB_NANO_FORMAT
                /* printf in newlib-nano does not support %ll format, causing example test fail */
                printf("Returned from sleep, reason: %s, t=%d ms, slept for %d ms\n",
                wakeup_reason, (int) (t_after_us / 1000), (int) ((t_after_us - t_before_us) / 1000));
            #else
                printf("Returned from sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);
            #endif
        
        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
            /* Waiting for the gpio inactive, or the chip will continously trigger wakeup*/
            example_wait_gpio_inactive();
        }
        
    }
    vTaskDelete(NULL);
}


void app_main(void)
{
    esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "myTimer_args",
        .skip_unhandled_events = true //sin este argumento no se comporta bien, acumula logs y los lanza todos de golpe al salir del sleep, también puede sacar del sleep sin necesidad de pulsar el gpio ni que pase el tiempo necesario.
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &myTimer));

    esp_timer_start_periodic(myTimer, 500000);  // Iniciar temporizador de alta resolución (500000 microsegundos = 0.5 segundos)
    
    /* Enable wakeup from light sleep by gpio */
    //example_register_gpio_wakeup();
    
    xTaskCreate(sleep_task, "sleep_task", 4096, NULL, 6, NULL);
}
