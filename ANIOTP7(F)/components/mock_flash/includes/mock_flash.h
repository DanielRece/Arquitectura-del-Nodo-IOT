/*
 * mock_flash.h
 *
 *  Created on: 10 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#ifndef COMPONENTS_MOCK_FLASH_INCLUDES_MOCK_FLASH_H_
#define COMPONENTS_MOCK_FLASH_INCLUDES_MOCK_FLASH_H_

#include <stdint.h>
#include <esp_check.h>


// Inicializa la flash interna con size bytes
esp_err_t initFlash(size_t size);

// Libera los recursos de la flash.
esp_err_t deinitFlash();


// Escribe size bytes de data
esp_err_t writeToFlash(void* data, size_t size);


// Devuelve la cantidad de datos pendientes por leer de la flash
size_t getDataLeft();


// Devuelve un puntero a los size bytes leídos de la flash
void* readFromFlash(size_t size);

#endif /* COMPONENTS_MOCK_FLASH_INCLUDES_MOCK_FLASH_H_ */
