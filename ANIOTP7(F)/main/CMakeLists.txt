idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS main.c i2c_config.c gpio_wakeup.c timer_wakeup.c 
						INCLUDE_DIRS "."
						EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)
