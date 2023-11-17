| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

Práctica realizada por Fabrizio Alcaraz & Daniel Rece

# _CUESTIONES_

## ¿Qué entradas tiene la tabla de particiones usada?

La tabla de particiones usada tiene las siguientes entradas:

nvs,      data, nvs,     0x9000,  0x4000,
otadata,  data, ota,     0xd000,  0x2000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000,  1M,
ota_0,    app,  ota_0,   0x110000, 1M,
ota_1,    app,  ota_1,   0x210000, 1M,

Pues utiliza la configuración “Factory app, two OTA definitions”.
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html

## ¿Cómo se llama el fichero de certificado que se incluirá en el binario?

El fichero de certificado que se incluirá en el binario se llama ca_cert.pem

## ¿Cómo y dónde se indica que se debe incluir el certificado?

La inclusión del certificado se indica en el CMakeLists.txt de la carpeta main con las siguientes líneas:

idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register( ...  EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)

## ¿Qué es el símbolo server_cert_pem_start?

El símbolo server_cert_pem_start indica el inicio de la región de memoria flash ocupada del certificado embebido