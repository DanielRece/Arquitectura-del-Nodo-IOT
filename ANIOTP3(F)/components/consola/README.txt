SI HAY ALGUNA DUDA O PROBELMA PORFAVOR PREGUNTADNOS: Jesus Espadas y Ovideo Zea

Como usar el componente consola:
1º añadir la carpeta consola a la carpeta components
2º hacer #include "consola.h" en tu main
3º se debe llamar consolaEvent_init() en el main para inicializar algunos parametros.
4º para iniciar el modo consola se debe llamar a la funcion consola_init() que pone en marcha el monitor, es muy aconsejable apagar todas las leturas de eventos realcionadas con la lectura de
temperatura antes de hacer consola_init() pues nada mas lanzar esta funcion se emepzara a leer por pantalla.
5º los comandos posibles son los siguientes: 
-help: muestra todos los comandos disponibles
-quota: retorna los bytes en la flash (IMPORTANTE: este comando llama a la funcion getDataLeft() del mock-flash, asegurese que esta bien agregada y revise la funcion en caso de problemas)
-monitor: lanza el evento CONSOLA_EVENTS_MONITORIZACION (de CONSOLA_EVENTS) y termina toda la lectura del monitor, aconsejamos registras el evento para reactivar los eventos desactivados
cuando se activo la consola.
6º tambien hemos creado el evento CONSOLA_EVENTS_CONSOLA por si lo quereis usar para manejar cuando activar la consola
7º tambien hemos creado un afuncion get_loop_consola_args() que retorna un esp_event_loop_args_t que podeis usar para registrar los eventos.

RECOMENDACIONES:
recomendamos usar esp_event_loop_args_t para registras los eventos a las tareas:
esp_event_handler_register(CONSOLA_EVENTS, CONSOLA_EVENTS_MONITORIZACION, monitor_event, &loop_args_consola_main);


void monitor_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG,"entrando en modo monitor\n");
    esp_event_handler_register([encender registros]);
    .
    .
    .
}

