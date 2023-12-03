#pragma once
#include "shared/utils.h"
#include "shared/memory.h"
#include <stddef.h>

void memory_phy_update_bmap();
physical_ptr_t memory_phy_allocate_aligned(size_t size);
void memory_phy_free(physical_ptr_t pointer, size_t size);
void memory_phy_reserve(physical_ptr_t pointer, size_t size);

void memory_virt_init();
void memory_virt_free(void *ptr, size_t size);
void memory_virt_reserve(void *ptr, size_t size);
void *memory_virt_allocate(size_t size);
void *memory_virt_create_phy_view(physical_ptr_t physicalAddress, size_t size);
