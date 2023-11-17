/*
 * ota_manager.c
 *
 *  Created on: 10 nov 2023
 *      Author: Fabrizio Alcaraz
 */
#include "ota_manager.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"


static const char *TAG = "OTA_MANAGER";
#define OTA_URL_SIZE 256

static short int ota_manager_self_test(){
	srand(time(NULL));
	return rand()%2;		// De esta forma se comprobaría el rollback la mitad de veces
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

static void ota_manager_update_task(void *pvParameter){
    ESP_LOGI(TAG, "Starting OTA update");
    esp_http_client_config_t config = {
        .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };


#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void ota_manager_update(void){
	//esp_wifi_set_ps(WIFI_PS_NONE);
	xTaskCreate(&ota_manager_update_task, "ota_manager_update_task", 8192, NULL, 5, NULL);
}

void ota_manager_partition_diagnosis(void){
	const esp_partition_t *running = esp_ota_get_running_partition();
	esp_ota_img_states_t ota_state;

	if(esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
		switch(ota_state){
		case ESP_OTA_IMG_NEW:
			ESP_LOGW(TAG,"Current ota_state is ESP_OTA_IMG_NEW");
			break;

		case ESP_OTA_IMG_PENDING_VERIFY:
			ESP_LOGW(TAG,"Current ota_state is ESP_OTA_IMG_PENDING_VERIFY");
			if (ota_manager_self_test()) {
				ESP_LOGI(TAG, "Diagnostics completed successfully! Continuing execution ...");
				esp_ota_mark_app_valid_cancel_rollback();
			} else {
				ESP_LOGE(TAG, "Diagnostics failed! Start rollback to the previous version ...");
				esp_ota_mark_app_invalid_rollback_and_reboot();
			}
			break;

		case ESP_OTA_IMG_VALID:
			ESP_LOGI(TAG,"Current ota_state is ESP_OTA_IMG_VALID");
			break;

		case ESP_OTA_IMG_INVALID:
			ESP_LOGE(TAG,"Current ota_state is ESP_OTA_IMG_INVALID");
			break;

		case ESP_OTA_IMG_ABORTED:
			ESP_LOGE(TAG,"Current ota_state is ESP_OTA_IMG_ABORTED");
			break;

		case ESP_OTA_IMG_UNDEFINED:
			ESP_LOGW(TAG,"Current ota_state is ESP_OTA_IMG_UNDEFINED");
			break;
		}
	}else {
		ESP_LOGE(TAG, "Can't read partition, start rollback to the previous version ...");
		esp_ota_mark_app_invalid_rollback_and_reboot();
	}
}

