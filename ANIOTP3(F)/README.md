| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

Práctica realizada por Fabrizio Alcaraz & Daniel Rece

# _CUESTIONES_

## ¿Qué prioridad tiene la tarea inicial que ejecuta la función app_main()? ¿Con qué llamada de ESP-IDF podemos conocer la prioridad de una tarea?
Tal y como se indica en la documentación de FreeRTOS del ESP32, la función app_main() tiene prioridad 1
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html

Para poder conocer la prioridad de una tarea bastaría con utilizar la llamada a uxTaskPriorityGet(const TaskHandle_t xTask).

## ¿Cómo sincronizas ambas tareas?¿Cómo sabe la tarea inicial que hay un nuevo dato generado por la tarea muestreadora?

Por la implementación realizada no hay sincronización entre ambas tareas, ya que la tarea inicial lee constantemente la variable global en donde
la tarea muestredora escribe su resultado.

## Si además de pasar el período como parámetro, quisiéramos pasar como argumento la dirección en la que la tarea muestreadora debe escribir las lecturas, ¿cómo pasaríamos los dos argumentos a la nueva tarea?

Para pasar los dos argumentos a la nueva tarea habría que crear un struct como por ejemplo:

struct{
    int delay,
    float *temp
} args_t;

Y, al crear la tarea pasar el struct como (void *)

## Al enviar un dato por una cola, ¿el dato se pasa por copia o por referencia?. Consulta la documentación para responder.

El dato se pasa por copia.
https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32/api-reference/system/freertos.html#queue-api

## ¿Qué debe hacer la tarea inicial tras registrar el handle? ¿Puede finalizar?

Una vez registrado el handle la tarea inicial debería de acabar con la llamada a vTaskDelete(NULL).