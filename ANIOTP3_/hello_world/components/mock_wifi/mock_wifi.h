// ---------------------------------------------------------------------------
// Implementa un componente WiFi simulado que permite conectar y desconectar de WiFi
// y enviar y recibir datos, simulando una tasa de fallos del 5 % de la conexión

#ifndef __MOCK_WIFI_H__
#define __MOCK_WIFI_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_OK_SEND_DATA   0xA0
#define WIFI_ERR_SEND_DATA  0xB0

// Parámetro de latencia de conexión, en segundos
#define ESP_WIFI_LATENCY        CONFIG_ESP_WIFI_LATENCY

// Parámetro de latencia de obtención de IP, en segundos
#define ESP_GOTIP_LATENCY       CONFIG_ESP_GOTIP_LATENCY

// Parámetro de tiempo de desconexión, en segundos
#define ESP_NETWORKDOWN_DELAY    CONFIG_ESP_NETWORKDOWN_DELAY

// Parámetro de tasa de conexión inicial con éxito, en porcentaje 0-100
#define ESP_GOTWIFI_RATE        CONFIG_ESP_GOTWIFI_RATE

ESP_EVENT_DECLARE_BASE(MOCK_WIFI_EVENTS);

typedef enum {
    WIFI_CONECTADO,
    WIFI_DESCONECTADO,
    WIFI_IP_CONSEGUIDA
} wifi_events_t;

/*
 * Trata simuladamente de conectar a la red WiFi.
 * - Si la conexión se produce, recibiremos un evento WIFI_CONECTADO.
 * - Si la conexión no se produce, recibiremos un evento WIFI_DESCONECTADO.
 * 
 * Una vez conseguida la conexión, tratará de conseguir una IP
 * automáticamente y se recibira el evento IP_CONSEGUIDA cuando se obtenga.
*/
void wifi_connect();

/*
 * Desconecta simuladamente la red WiFi. Inicializa a NULL la dirección IP
 * y genera el evento WIFI_DESCONECTADO.
 */
void wifi_disconnect();

/*
 * Permite simuladamente enviar un dato mediante la conexión WiFI.
 * Devolverá un error si el envío no se pudo realizar.
 * Imprimirá el dato por puerto serie (se asume que el dato de entrada es
 * es una cadena de caracteres).
*/
esp_err_t send_data_wifi(void* data, size_t size);

/*
 * Devuelve un puntero a la dirección IP
 */
char *get_ip_wifi();

#ifdef __cplusplus
}
#endif

#endif /* __MOCK_WIFI_H__ */
