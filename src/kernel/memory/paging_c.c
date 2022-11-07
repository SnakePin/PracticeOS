#include <stdint.h>
#include "memory/paging.h"
#include "memory/memory.h"

CDECL_ATTR void paging_load_dir(void *pageDir);
static void map_internal(physical_ptr_t phyAddress, size_t length, void *virtAddr, uint16_t attribute);

PagingStructure_t *paging_pPagingStruct;

static void map_internal(physical_ptr_t phyAddress, size_t length, void *virtAddr, uint16_t attribute)
{
    for (size_t i = 0; i < PAGE_DIR_LENGTH; i++)
    {
        paging_pPagingStruct->directory[i] = ((PageDirectoryEntry_t)&paging_pPagingStruct->tables[i]) | 3; // supervisor level, read/write, present
    }

    physical_ptr_t currentPhysicalPage = ADDR_TO_PAGE(phyAddress);
    physical_ptr_t currentPage = ADDR_TO_PAGE(virtAddr);
    for (size_t i = 0; i < LEN_TO_PAGE(length); i++)
    {
        size_t dirIndex = currentPage / PAGE_TABLE_LENGTH;
        size_t tableIndex = currentPage % PAGE_TABLE_LENGTH;
        paging_pPagingStruct->tables[dirIndex][tableIndex] = (currentPhysicalPage << 12) | attribute;

        currentPage++;
        currentPhysicalPage++;
    }
}

void paging_load_structure(PagingStructure_t *pPagingStruct)
{
    paging_pPagingStruct = pPagingStruct;
    paging_update_table();
}

void paging_map(physical_ptr_t phyAddress, void *virtAddr, size_t length)
{
    map_internal(phyAddress, length, virtAddr, 0x3); // supervisor level, read/write, present
}

void paging_unmap(void *virtAddr, size_t length)
{
    map_internal(0, length, virtAddr, 0x2); // supervisor level, read/write, not present
}

void paging_update_table()
{
    paging_load_dir(&paging_pPagingStruct->directory);
}
