# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES "driver" "esp_timer" "esp_wifi" "nvs_flash" "si7021")
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c" "i2c_config.c" INCLUDE_DIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
