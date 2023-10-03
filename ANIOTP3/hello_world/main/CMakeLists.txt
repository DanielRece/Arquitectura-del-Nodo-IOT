set(COMPONENT_REQUIRES "esp_flash")

idf_component_register(SRCS "hello_world_main.c" INCLUDE_DIRS "" REQUIRES "driver")
