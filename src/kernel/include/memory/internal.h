#pragma once
#include "memory/memory.h"
#include <stdint.h>

#define ACPI_E820_INT 0x15
#define ACPI_E820_EAX 0xE820
#define ACPI_E820_BUFFER INT_GATE16_FREE_MEM
#define ACPI_E820_BUFFER_SIZE 24       // ACPI3.0 entry size
#define ACPI_E820_SIGNATURE 0x534D4150 //'SMAP'
#define ACPI_MEMORY_MAP_LENGTH 128

typedef struct {
    uint64_t BaseAddress;
    uint64_t Length;
    uint32_t Type;
} PACKED_ATTR MemoryACPIMapEntry_t;

enum MemoryACPIMapEntryType {
    Usable = 1,
    Reserved = 2,
    /*
    * ACPI Reclaim Memory. 
    * This range is available RAM usable by the OS after it reads the ACPI tables.
    */
    ACPIReclaimable = 3,
    /*
    * ACPI NVS Memory.
    * This range of addresses is in use or reserved by the system and must not be used by the operating system.
    * This range is required to be saved and restored across an NVS sleep.
    */
   ACPINVS = 4,
   UnusableBadMemory = 5,
   Undefined
};
