#pragma once
#include <stdint.h>
#include "utils.h"

#define PAGE_SIZE 0x1000
#define PAGE_DIR_LENGTH 1024
#define PAGE_TABLE_LENGTH 1024

typedef uint32_t PageDirectoryEntry_t;
typedef PageDirectoryEntry_t *PPageDirectoryEntry_t;
typedef uint32_t (*PPageTable_t)[PAGE_TABLE_LENGTH];

void paging_enable_paging();
void paging_update_table();
void paging_map(uintptr_t phyAddress, size_t length, uintptr_t virtAddr);
void paging_unmap(uintptr_t phyAddress, size_t length, uintptr_t virtAddr);
void paging_init(PPageDirectoryEntry_t _pPageDirectory, PPageTable_t _pPageTableArray);
