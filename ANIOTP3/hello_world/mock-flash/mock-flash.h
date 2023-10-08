#ifndef MOCK_FLASH_H
#define MOCK_FLASH_H

#include <stdint.h>

typedef struct {
    uint8_t* buffer;
    size_t capacity;
    size_t head;
    size_t tail;
} CircularBuffer;

esp_err_t circularBuffer_init(CircularBuffer* buffer, size_t capacity);
esp_err_t writeToFlash(CircularBuffer* buffer, void* data, size_t size);
void* readFromFlash(CircularBuffer* buffer, size_t size);
size_t getDataLeft(CircularBuffer* buffer);
void circularBuffer_destroy(CircularBuffer* buffer);

#endif // MOCK_FLASH_H
