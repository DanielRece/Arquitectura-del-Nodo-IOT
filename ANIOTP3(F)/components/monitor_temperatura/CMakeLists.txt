idf_component_register(SRCS "monitor_temperatura.c"
INCLUDE_DIRS "includes"
REQUIRES driver si7021 esp_event esp_timer
)