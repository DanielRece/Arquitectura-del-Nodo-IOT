/*
 * mock_flash.c
 *
 *  Created on: 10 oct 2023
 *      Author: Fabrizio Alcaraz & Daniel Rece
 */

#include "mock_flash.h"

#include <string.h>

typedef struct buffer_t{
	uint8_t* data;
	size_t size;
	size_t head;
	size_t tail;
	size_t len;
} buffer_t;

struct buffer_t buffer;

esp_err_t initFlash(size_t size){

	if (size <= 0) return ESP_ERR_INVALID_SIZE;
	buffer.size = size;

	buffer.data = malloc(size);

	if (buffer.data == NULL) return ESP_ERR_NO_MEM;

	buffer.head = 0;
	buffer.tail = 0;

	buffer.len = 0;

	return ESP_OK;
}

esp_err_t deinitFlash(){
	free(buffer.data);
	buffer.size = 0;
	buffer.len = 0;
	buffer.head = 0;
	buffer.tail = 0;

	return ESP_OK;
}

esp_err_t writeToFlash(void* data, size_t size){
	size_t aux;

	if (size <= 0) return ESP_ERR_INVALID_SIZE;
	else if (size >= (buffer.size - buffer.len)) return ESP_ERR_NO_MEM; // Se comprueba que haya espacio suficiente

	aux = buffer.tail - buffer.size; // Se comprueba si se está al final

	if (aux > size){
		memcpy(buffer.data + buffer.tail, data, size);
	}else {
		memcpy(buffer.data + buffer.tail, data, aux);
		memcpy(buffer.data, (uint8_t*)data + aux, size - aux);
	}

	buffer.tail = (buffer.tail + size) % buffer.size;
	buffer.len += size;

	return ESP_OK;
}

void* readFromFlash(size_t size){
	size_t aux;
	uint8_t *data;
	if (size <= 0) return NULL;
	else if (size > buffer.len) return NULL;

	aux = buffer.head - buffer.size; // Se comprueba si se está al final

	data = malloc(size);

	if(aux > size){
		memcpy(data ,buffer.data + buffer.head, size);

	}else {
		memcpy(data, buffer.data + buffer.head,  aux);
		memcpy(data + aux, buffer.data,  size - aux);
	}


	buffer.head = (buffer.head + size) % buffer.size;
	buffer.len -= size;

	return data;

}

size_t getDataLeft(){
	return buffer.len;
}

void printFlash(void){
    printf("Size: %u \n", buffer.size );
    printf("Head: %u \n", buffer.head );
    printf("Tail: %u \n", buffer.tail );
    printf("Len: %u \n", buffer.len );
}


