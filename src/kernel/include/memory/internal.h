#pragma once
#include "memory/memory.h"

#define ACPI_E820_INT 0x15
#define ACPI_E820_EAX 0xE820
#define ACPI_E820_BUFFER INT_GATE16_FREE_MEM
#define ACPI_E820_BUFFER_SIZE 24       // ACPI3.0 entry size
#define ACPI_E820_SIGNATURE 0x534D4150 //'SMAP'
#define ACPI_MEMORY_MAP_LENGTH 128

extern size_t memory_MemoryMapEntryCount;
extern MemoryACPIMapEntry_t memory_MemoryMapACPI[ACPI_MEMORY_MAP_LENGTH];
extern MemoryPhyAllocLink_t memory_PhyAllocListRoot;

//First-fit functions
uintptr_t phy_get_firstfit(size_t size);
