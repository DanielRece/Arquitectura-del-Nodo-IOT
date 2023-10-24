idf_component_register(SRCS "src/consola.c"
                    INCLUDE_DIRS "include" "../mock_flash"
                    REQUIRES console spi_flash esp_event esp_timer mock_flash)
