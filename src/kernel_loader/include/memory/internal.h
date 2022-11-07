#pragma once
#include "memory/memory.h"
#include <stdint.h>

#define ACPI_E820_INT 0x15
#define ACPI_E820_EAX 0xE820
#define ACPI_E820_BUFFER INT_GATE16_FREE_MEM
#define ACPI_E820_BUFFER_SIZE 24       // ACPI3.0 entry size
#define ACPI_E820_SIGNATURE 0x534D4150 //'SMAP'
#define ACPI_MEMORY_MAP_LENGTH 128

typedef uint8_t MemoryBitmapValue_t;
#define MEM_BITMAP_VALUE_PAGE_COUNT (sizeof(MemoryBitmapValue_t)*8)
#define MEM_BITMAP_PAGE_COUNT ((32*1024*1024) / PAGE_SIZE) // Should be 32MiB
#define MEM_BITMAP_LENGTH (MEM_BITMAP_PAGE_COUNT / MEM_BITMAP_VALUE_PAGE_COUNT)
