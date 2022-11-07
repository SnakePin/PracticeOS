#pragma once
#include <stdint.h>
#include "utils.h"
#include "memory/memory.h"

#define PAGE_SIZE 0x1000
#define PAGE_DIR_LENGTH 1024
#define PAGE_TABLE_LENGTH 1024
#define LEN_TO_PAGE(x) (ALIGN_ADDRESS_CEIL((x), PAGE_SIZE) / PAGE_SIZE)
#define ADDR_TO_PAGE(x) (ALIGN_ADDRESS_FLOOR((x), PAGE_SIZE) / PAGE_SIZE)

typedef uint32_t PageDirectoryEntry_t;
typedef uint32_t PageTableEntry_t;

typedef struct
{
    PageDirectoryEntry_t directory[PAGE_DIR_LENGTH];
    PageTableEntry_t tables[PAGE_TABLE_LENGTH][PAGE_DIR_LENGTH];
} PACKED_ATTR PagingStructure_t;

void paging_update_table();
void paging_map(physical_ptr_t phyAddress, void* virtAddr, size_t length);
void paging_unmap(void* virtAddr, size_t length);
void paging_load_structure(PagingStructure_t *pPagingStruct);
