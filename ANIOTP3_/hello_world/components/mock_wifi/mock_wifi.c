#include "mock_wifi.h"

ESP_EVENT_DEFINE_BASE(MOCK_WIFI_EVENTS);

static wifi_events_t wifi_status;
static uint8_t seconds_to_fail;
static const char *TAG = "mock-wifi";

static void ip_timer_callback(void* arg);
static void network_down_timer_callback(void* arg);

/*
 * Inicializa el componente WiFi 
*/
void wifi_init() {
    wifi_status = WIFI_DESCONECTADO;
    seconds_to_fail = 0;
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
    int success = 0;

    srand(0);
    success = 100 * rand() / RAND_MAX;

    // El retraso en vTaskDelay se especifica en ticks; para convertir a
    // segundos hay que multiplicar por 1000 y dividir por portTICK_PERIOD_MS 
    vTaskDelay(1000 * ESP_WIFI_LATENCY / portTICK_PERIOD_MS);
    if (success < ESP_GOTWIFI_RATE) {
        wifi_status = WIFI_CONECTADO;
        esp_timer_create_args_t ip_timer_args = {
            .callback = &ip_timer_callback,
            .name = "ip timer task",
        };
        esp_timer_handle_t ip_timer_handle;
        ESP_ERROR_CHECK(esp_timer_create(&ip_timer_args, &ip_timer_handle));
        // Incluimos una referencia al handler como argumento pasado;
        ip_timer_args.arg = (void *) &ip_timer_handle;
        // El retraso en esp_timer_start_once se especifica en microsegundos; para
        // convertir a segundos hay que multiplicar por 10^6
        ESP_ERROR_CHECK(esp_timer_start_once(ip_timer_handle, ESP_GOTIP_LATENCY * 1000000));
    } else {
        wifi_status = WIFI_DESCONECTADO;
    }
    esp_event_post(MOCK_WIFI_EVENTS, wifi_status, NULL, 0, 100);
}

/*
 * Desconecta simuladamente la red WiFi. Inicializa a NULL la dirección IP
 * y genera el evento WIFI_DESCONECTADO.
 */
void wifi_disconnect() {
    wifi_status = WIFI_DESCONECTADO;
    esp_event_post(MOCK_WIFI_EVENTS, WIFI_DESCONECTADO, NULL, 0, 100);
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

static void ip_timer_callback(void* arg) {
    wifi_status = WIFI_IP_CONSEGUIDA;
    esp_event_post(MOCK_WIFI_EVENTS, WIFI_IP_CONSEGUIDA, NULL, 0, 100);
    ESP_LOGI(TAG, "WiFi operativa con dirección IP %s.\n", get_ip_wifi());

    esp_timer_create_args_t network_down_timer_args = {
        .callback = &network_down_timer_callback,
        .name = "network down timer task",
    };
    esp_timer_handle_t network_down_timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&network_down_timer_args,
                        &network_down_timer_handle));
    // Incluimos una referencia al handler como argumento pasado;
    network_down_timer_args.arg = (void *) &network_down_timer_handle;
    // El retraso en esp_timer_start_once se especifica en microsegundos; para
    // convertir a segundos hay que multiplicar por 10^6
    ESP_ERROR_CHECK(esp_timer_start_once(network_down_timer_handle,
                        ESP_NETWORKDOWN_DELAY * 1000000));

    // ESP_ERROR_CHECK(esp_timer_delete((esp_timer_handle_t) arg));
}

static void network_down_timer_callback(void* arg) {
    wifi_disconnect();
    // ESP_ERROR_CHECK(esp_timer_delete((esp_timer_handle_t) arg));
}
