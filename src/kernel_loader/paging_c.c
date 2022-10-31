#include <stdint.h>
#include "paging.h"

CDECL_ATTR void paging_load_dir(void *pageDir);
CDECL_ATTR void paging_enable();
static void identity_map_all();
static void map_internal(uintptr_t phyAddress, size_t length, uintptr_t virtAddr, uint16_t attribute);

static PPageDirectoryEntry_t pPageDirectory;
static PPageTable_t pPageTableArray;

static void identity_map_all()
{
    for (size_t i = 0; i < PAGE_DIR_LENGTH; i++)
    {
        for (size_t j = 0; j < PAGE_TABLE_LENGTH; j++)
        {
            uintptr_t currentPhysicalPage = i * PAGE_TABLE_LENGTH + j;
            pPageTableArray[i][j] = (currentPhysicalPage << 12) | 3; // supervisor level, read/write, present
        }
        pPageDirectory[i] = ((uintptr_t)&pPageTableArray[i]) | 3; // supervisor level, read/write, present
    }
}

static void map_internal(uintptr_t phyAddress, size_t length, uintptr_t virtAddr, uint16_t attribute)
{
    uintptr_t currentPhysicalPage = ALIGN_ADDRESS_FLOOR(phyAddress, PAGE_SIZE) / PAGE_SIZE;
    uintptr_t currentPage = ALIGN_ADDRESS_FLOOR(virtAddr, PAGE_SIZE) / PAGE_SIZE;
    uintptr_t lengthInPages = ALIGN_ADDRESS_CEIL(length, PAGE_SIZE) / PAGE_SIZE;
    for (size_t i = 0; i < lengthInPages; i++)
    {
        size_t dirIndex = currentPage / PAGE_TABLE_LENGTH;
        size_t tableIndex = currentPage % PAGE_TABLE_LENGTH;

        PPageTable_t pageTablePointer = (PPageTable_t)((uintptr_t)pPageDirectory[dirIndex] & ~0xFFF);
        (*pageTablePointer)[tableIndex] = (currentPhysicalPage << 12) | attribute;

        currentPage++;
        currentPhysicalPage++;
    }
}

void paging_init(PPageDirectoryEntry_t _pPageDirectory, PPageTable_t _pPageTableArray)
{
    pPageTableArray = _pPageTableArray;
    pPageDirectory = _pPageDirectory;
    identity_map_all();
}

void paging_map(uintptr_t phyAddress, size_t length, uintptr_t virtAddr)
{
    map_internal(phyAddress, length, virtAddr, 0x3); // supervisor level, read/write, present
}

void paging_unmap(uintptr_t phyAddress, size_t length, uintptr_t virtAddr)
{
    map_internal(phyAddress, length, virtAddr, 0x2); // supervisor level, read/write, not present
}

void paging_update_table()
{
    paging_load_dir(pPageDirectory);
}

void paging_enable_paging()
{
    paging_update_table();
    paging_enable();
}
