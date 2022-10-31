#pragma once
#include <stdint.h>
#include <stddef.h>

uint8_t lba_read_from_disk(uint8_t diskID, uint32_t lbaOffset, uint64_t byteOffset, size_t length, void *outBuffer);
