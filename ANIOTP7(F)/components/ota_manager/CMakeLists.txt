idf_component_register(SRCS "ota_manager.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_https_ota app_update mbedtls)

