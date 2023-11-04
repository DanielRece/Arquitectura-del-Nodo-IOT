#include "consola.h"

static const char *TAG= "consola";

void register_monitor(void);
void register_quota(void);
esp_console_repl_t *repl = NULL;
ESP_EVENT_DEFINE_BASE(CONSOLA_EVENTS);


esp_event_loop_args_t loop_args_consola = {
        .queue_size = 5,
        .task_name = "loop_task",
        .task_priority = 7,
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY
    };


void register_commands(void)
{
    register_monitor();
    register_quota();
}

void consola_stop() 
{
    esp_console_deinit();
}

static void activar_monitorizacion(int argc, char **argv)
{
    ESP_LOGI(TAG, "Activa monitorizacion \n");
    esp_event_post(CONSOLA_EVENTS, CONSOLA_EVENTS_MONITORIZACION, NULL, 0, portMAX_DELAY);
    esp_console_deinit();
    repl->del(repl);
}

void register_monitor(void)
{
    const esp_console_cmd_t cmd = {
        .command = "monitor",
        .help = "Activa el modo monitorización para el sensor de temperatura y humedad",
        .hint = NULL,
        .func = &activar_monitorizacion,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

void get_quota(int argc, char **argv)
{   
    /*CircularBuffer m_Buffer_Temperatura_local;
    size_t dataSize = getDataLeft(&m_Buffer_Temperatura_local);
    ESP_LOGI(TAG, "Imagenos que es %d \n",dataSize);*/    

    size_t dataSize = getDataLeft();
    ESP_LOGI(TAG, "Imagenos que es %d \n",dataSize);
}

void register_quota(void)
{
    const esp_console_cmd_t cmd = {
        .command = "quota",
        .help = "Retorna la cantidad de Bytes por leer de la memoria flash",
        .hint = NULL,
        .func = &get_quota,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

void consolaEvent_init()
{
    esp_event_loop_create_default();
}

void consola_init()
{
    
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "command ->";
    repl_config.max_cmdline_length = 1024;

    esp_console_register_help_command();
    register_commands();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

esp_event_loop_args_t get_loop_consola_args()
{
    return loop_args_consola;
}
