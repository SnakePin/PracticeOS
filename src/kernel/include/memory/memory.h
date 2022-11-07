#pragma once
#include "utils.h"
#include <stdint.h>
#include <stddef.h>

#define PHY_NULL 0

//This must be changed if we want to support PAE in 32-bit mode
typedef uintptr_t physical_ptr_t;

void memory_phy_init();
physical_ptr_t memory_phy_allocate_aligned(size_t size);
void memory_phy_free(physical_ptr_t pointer, size_t size);
void memory_phy_reserve(physical_ptr_t pointer, size_t size);

void memory_virt_init();
void memory_virt_free(void *ptr, size_t size);
void *memory_virt_allocate(size_t size);
