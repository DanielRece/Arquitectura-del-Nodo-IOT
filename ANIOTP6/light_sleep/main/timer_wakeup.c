/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_check.h"
#include "esp_sleep.h"

#define TIMER_WAKEUP_TIME_US_LIGHT    (5 * 1000 * 1000)

#define TIMER_WAKEUP_TIME_US_DEEP    (60 * 1000 * 1000)

static const char *TAG = "timer_wakeup";

esp_err_t example_register_light_sleep_timer_wakeup(void)
{
    ESP_RETURN_ON_ERROR(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US_LIGHT), TAG, "Configure timer as wakeup source failed");
    return ESP_OK;
}

esp_err_t example_register_deep_sleep_timer_wakeup(void)
{
    ESP_RETURN_ON_ERROR(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US_DEEP), TAG, "Configure timer as wakeup source failed");
    return ESP_OK;
}
