#include <stdint.h>
#include "lba_disk.h"
#include "utils.h"
#include "elf.h"
#include "memory/paging.h"
#include "memory/memory.h"
#include "memory/internal.h"

extern MemoryBitmapValue_t mem_bitmap[];

CDECL_ATTR NORETURN_ATTR void jump_to_kernel(void *code, void* newStack, void *arg1, void *arg2, void *arg3);

// TODO: make this an actual file instead
#define KERNEL_ELF_FILE_OFFSET 0x20000
#define KERNEL_ELF_FILE_LEN_OFFSET 0x1FFFC

// Has BIOS structures, loader and loader's stack
#define MEM_RESERVED 0
#define MEM_RESERVED_LEN 0x100000

#define KERNEL_STACK_SIZE 0x100000

static void *MapELFAndGetEntryPoint(uint8_t *pFile);

CDECL_ATTR void ldrmain(uint8_t bootDiskID, uint32_t bootPartitionLBA)
{
    memory_phy_init();
    memory_phy_reserve(MEM_RESERVED, MEM_RESERVED_LEN);

    PagingStructure_t *pPagingStructure = (PagingStructure_t *)memory_phy_allocate_aligned(sizeof(PagingStructure_t));
    paging_load_structure(pPagingStructure);
    for (size_t i = 0; i < PAGE_DIR_LENGTH; i++)
    {
        pPagingStructure->directory[i] = ((PageDirectoryEntry_t)&pPagingStructure->tables2D[i]) | 3; // supervisor level, read/write, present
    }
    paging_map(0, 0, MEM_BITMAP_PAGE_COUNT * PAGE_SIZE);
    paging_enable_paging(); // All allocatable pages(32MiB) are identity mapped from now on

    uint32_t fileSize;
    lba_read_from_disk(bootDiskID, bootPartitionLBA, KERNEL_ELF_FILE_LEN_OFFSET, sizeof(uint32_t), &fileSize);

    uint8_t *pPhyKernelElf = (uint8_t *)memory_phy_allocate_aligned(fileSize);
    lba_read_from_disk(bootDiskID, bootPartitionLBA, KERNEL_ELF_FILE_OFFSET, fileSize, pPhyKernelElf);
    void *entryPoint = MapELFAndGetEntryPoint(pPhyKernelElf);
    // ELF parsing is done, free the memory
    memory_phy_free((physical_ptr_t)pPhyKernelElf, fileSize);

    if (entryPoint == NULL)
    {
        return; // ELF Parsing failed
    }

    size_t bitmapSize = MEM_BITMAP_LENGTH * sizeof(MemoryBitmapValue_t);
    physical_ptr_t newBitmap = memory_phy_allocate_aligned(bitmapSize);
    memcpy((void *)newBitmap, mem_bitmap, bitmapSize);

    // Allocate a stack for the kernel
    physical_ptr_t kernelStack = memory_phy_allocate_aligned(KERNEL_STACK_SIZE);

    jump_to_kernel(entryPoint, (void*)kernelStack, (void *)newBitmap, (void *)MEM_BITMAP_LENGTH, (void *)pPagingStructure);
    __builtin_unreachable();
}

static void *MapELFAndGetEntryPoint(uint8_t *pFile)
{
    Elf32_Ehdr *pELFHeader = (Elf32_Ehdr *)pFile;

    if (memcmp(pELFHeader->e_ident, ELF_MAGIC, ELF_MAGIC_LEN) != 0)
    {
        return NULL; // Invalid ELF
    }

    size_t numProgHeaders = pELFHeader->e_phnum;
    if (numProgHeaders == PN_XNUM)
    {
        if (pELFHeader->e_shnum == 0)
        {
            return NULL; // Invalid ELF
        }
        Elf32_Shdr *firstSectHeader = (Elf32_Shdr *)(pFile + pELFHeader->e_shoff);
        numProgHeaders = firstSectHeader->sh_info;
    }

    for (size_t i = 0; i < numProgHeaders; i++)
    {
        Elf32_Phdr *curProgHeader = (Elf32_Phdr *)(pFile + pELFHeader->e_phoff + i * pELFHeader->e_phentsize);

        if (curProgHeader->p_type == PT_LOAD)
        {
            physical_ptr_t alignedSegmentSpace = memory_phy_allocate_aligned(curProgHeader->p_memsz);
            physical_ptr_t segmentData = (physical_ptr_t)(pFile + curProgHeader->p_offset);

            memset((void *)alignedSegmentSpace, 0, curProgHeader->p_memsz);
            memcpy((void *)alignedSegmentSpace, (void *)segmentData, curProgHeader->p_filesz);
            paging_map(alignedSegmentSpace, (void *)curProgHeader->p_vaddr, curProgHeader->p_memsz);
        }
    }

    paging_update_table();
    return (void *)pELFHeader->e_entry;
}
