#include <stdint.h>
#include "shared/memory.h"
#include "shared/paging.h"

static void map_internal(physical_ptr_t phyAddress, size_t length, void *virtAddr, uint16_t attribute);

PagingStructure_t *paging_pPagingStruct;

static void map_internal(physical_ptr_t phyAddress, size_t length, void *virtAddr, uint16_t attribute)
{
    physical_ptr_t currentPhysicalPage = ADDR_TO_PAGE(phyAddress);
    physical_ptr_t currentPage = ADDR_TO_PAGE(virtAddr);
    for (size_t i = 0; i < LEN_TO_PAGE(length); i++)
    {
        paging_pPagingStruct->tables[currentPage] = (currentPhysicalPage << 12) | attribute;

        currentPage++;
        currentPhysicalPage++;
    }
}

void paging_load_structure(PagingStructure_t *pPagingStruct)
{
    paging_pPagingStruct = pPagingStruct;
    paging_update_table();
}

size_t paging_save_tables(void *virtAddr, size_t length, PageTableEntry_t *buffer, size_t bufferSize)
{
    size_t requiredBufferSize = LEN_TO_PAGE(length) * sizeof(PageTableEntry_t);
    if (bufferSize < requiredBufferSize)
    {
        return requiredBufferSize;
    }

    memcpy(buffer, &paging_pPagingStruct->tables[ADDR_TO_PAGE(virtAddr)], requiredBufferSize);
    return 0;
}

void paging_restore_tables(void *virtAddr, size_t length, PageTableEntry_t *buffer)
{
    size_t size = LEN_TO_PAGE(length) * sizeof(PageTableEntry_t);
    memcpy(&paging_pPagingStruct->tables[ADDR_TO_PAGE(virtAddr)], buffer, size);
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
