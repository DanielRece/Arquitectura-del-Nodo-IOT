#include "mock_wifi.h"

ESP_EVENT_DEFINE_BASE(MOCK_WIFI_EVENTS);

static wifi_events_t wifi_status;
static const char *TAG = "mock-wifi";



#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT){
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "starting wifi connection");
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_STOP:
                ESP_LOGI(TAG, "network interface finished");
                break;
            case WIFI_EVENT_STA_CONNECTED:
            	s_retry_num = 0;
            	wifi_status = WIFI_CONECTADO;
            	esp_event_post(MOCK_WIFI_EVENTS, wifi_status, NULL, 0, 100);
                ESP_LOGI(TAG, "waiting for an IP adress");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
            	wifi_status = WIFI_DESCONECTADO;
            	esp_event_post(MOCK_WIFI_EVENTS, wifi_status, NULL, 0, 100);
                if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
                    ESP_LOGW(TAG, "Wifi not connected");
                    esp_wifi_connect();
                    s_retry_num++;
                    ESP_LOGI(TAG, "retrying to connect to the AP");
                } else {
                    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
                }
                ESP_LOGE(TAG,"connect to the AP fail, no more tries");
                break;
        default:
            break;
        }
    }
}

static void ip_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	wifi_status = WIFI_IP_CONSEGUIDA;
	esp_event_post(MOCK_WIFI_EVENTS, wifi_status, NULL, 0, 100);
}

/*
 * Inicializa el componente WiFi 
*/
void wifi_init() {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ip_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/*
 * Trata simuladamente de conectar a la red WiFi.
 * - Si la conexión se produce, recibiremos un evento WIFI_CONECTADO.
 * - Si la conexión no se produce, recibiremos un evento WIFI_DESCONECTADO.
 * 
 * Una vez conseguida la conexión, tratará de conseguir una IP
 * automáticamente y se recibira el evento IP_CONSEGUIDA cuando se obtenga.
*/
void wifi_connect() {
	s_retry_num = 0;
	esp_wifi_connect();
}

/*
 * Desconecta simuladamente la red WiFi. Inicializa a NULL la dirección IP
 * y genera el evento WIFI_DESCONECTADO.
 */
void wifi_disconnect() {
	s_retry_num = EXAMPLE_ESP_MAXIMUM_RETRY;
   esp_wifi_disconnect();
}

/*
 * Permite simuladamente enviar un dato mediante la conexión WiFI.
 * Devolverá un error si el envío no se pudo realizar.
 * Imprimirá el dato por puerto serie (se asume que el dato de entrada es
 * es una cadena de caracteres).
*/
esp_err_t send_data_wifi(void* data, size_t size) {

    if (wifi_status == WIFI_IP_CONSEGUIDA) {
        ESP_LOGI(TAG, "%.*s", size, (char *) data);
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

/*
 * Devuelve un puntero a la dirección IP
 */
char *get_ip_wifi() {
    if (wifi_status == WIFI_IP_CONSEGUIDA) {
        return "192.168.1.3";
    } else {
        return "";
    }
}
