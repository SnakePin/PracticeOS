#pragma once
#include "utils.h"
#include <stdint.h>
#include <stddef.h>

void memory_init();

uintptr_t memory_phy_allocate(size_t size);
void memory_phy_free(uintptr_t pointer);

//int_gate16 exists here. The kernel and it's stack does so too until we improve the loader code.
#define MEMORY_KERNEL_RESERVED 0x3FFFFF //First 4MiB of the RAM

//This must be changed if we want to support PAE in 32-bit mode
typedef struct {
    uintptr_t BaseAddress;
    size_t Length;
    void* Next;
    void* Prev;
} MemoryPhyAllocLink_t;

typedef struct {
    uint64_t BaseAddress;
    uint64_t Length;
    uint32_t Type;
} PACKED_ATTR MemoryACPIMapEntry_t;

enum MemoryACPIMapEntryType_t {
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
