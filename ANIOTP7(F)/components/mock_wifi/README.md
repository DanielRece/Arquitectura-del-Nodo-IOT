| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# _Mock WiFi_ Grupo 2

Implementa un componente que simula el establecimiento y cierre de una conexión WiFi y permite
enviar y recibir datos, simulando una tasa de fallos por defecto del 5 % en el establecimiento
y con una caída de red por defecto cada 20 segundos.

Creado por Aldair Fredy Maldonado Honores y Ricardo Palomares Martínez

## Cómo usar el componente
Una vez instanciado el componente y registrado el manejador de eventos lanzados al bucle por
defecto, se puede simular el establecimiento de una conexion con la llamada no bloqueante
wifi_connect(), que lanzará el evento WIFI_CONECTADO si la conexión tiene éxito y
WIFI_DESCONECTADO si no lo tiene. Si se conecta a WiFi, se obtendrá también un evento IP_CONSEGUIDA
cuando se obtenga una dirección IP, lo que ocurre por defecto un segundo después de conseguir la WiFi.
Se puede obtener una cadena con la dirección IP (que, de momento, es siempre la misma) llamando
a get_ip_wifi().

Se podrán enviar datos con la función esp_err_t send_data_wifi(void* data, size_t size), que
devuelve WIFI_OK_SEND_DATA o WIFI_ERR_SEND_DATA. Ejemplo:

send_data_wifi("Datos a enviar", strlen("Datos a enviar"));
send_data_wifi("Datos a enviar", 0); // Ambas llamadas hacen lo mismo

Periódicamente (cada 20 segundos por defecto) se simula una caída de la conexión de red, lo
que provoca que lance el evento WIFI_DESCONECTADO, igual que si se llamara a wifi_disconnect().
