#pragma once
#include "utils.h"
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000
#define PHY_NULL 0

//This must be changed if we want to support PAE in 32-bit mode
typedef uintptr_t physical_ptr_t;

void memory_phy_init();
physical_ptr_t memory_phy_allocate_aligned(size_t size);
void memory_phy_free(physical_ptr_t pointer, size_t size);
void memory_phy_reserve(physical_ptr_t pointer, size_t size);

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
