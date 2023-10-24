#include "esp_console.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "mock_flash.h"


ESP_EVENT_DECLARE_BASE(CONSOLA_EVENTS);        // declaration of the SI7021 events family

enum {                                       // declaration of the specific events under the SI7021 event family
    CONSOLA_EVENTS_MONITORIZACION,
    CONSOLA_EVENTS_CONSOLA               
};


void consola_init();
void consolaEvent_init();
esp_event_loop_args_t get_loop_consola_args();
esp_event_loop_args_t loop_args_exitConsola();
