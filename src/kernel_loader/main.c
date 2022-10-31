#include <stdint.h>
#include "lba_disk.h"
#include "utils.h"
#include "elf.h"
#include "paging.h"

CDECL_ATTR NORETURN_ATTR void jump_to_code(void *code);

// TODO: make this an actual file instead
#define KERNEL_ELF_FILE_OFFSET 0x20000
#define KERNEL_ELF_FILE_LEN_OFFSET 0x1FFFC

#define PAGE_DIR_PHY_ADDR 0x200000 // Starts at 3rd MiB
#define PAGE_TABLES_PHY_ADDR (PAGE_DIR_PHY_ADDR + 0x1000)
#define KERNEL_ELF_LOAD_PHY_ADDR (PAGE_TABLES_PHY_ADDR + 0x400000)

CDECL_ATTR void ldrmain(uint8_t bootDiskID, uint32_t bootPartitionLBA)
{
    paging_init((PPageDirectoryEntry_t)PAGE_DIR_PHY_ADDR, (PPageTable_t)PAGE_TABLES_PHY_ADDR);
    paging_enable_paging();
    // All pages are identity mapped for now

    uint8_t *pPhyKernelElf = (uint8_t *)KERNEL_ELF_LOAD_PHY_ADDR;
    uint32_t fileSize;
    lba_read_from_disk(bootDiskID, bootPartitionLBA, KERNEL_ELF_FILE_LEN_OFFSET, sizeof(uint32_t), &fileSize);
    lba_read_from_disk(bootDiskID, bootPartitionLBA, KERNEL_ELF_FILE_OFFSET, fileSize, pPhyKernelElf);

    Elf32_Ehdr *pELFHeader = (Elf32_Ehdr *)pPhyKernelElf;

    if (memcmp(pELFHeader->e_ident, ELF_MAGIC, ELF_MAGIC_LEN) != 0)
    {
        return; // Invalid ELF
    }

    size_t numProgHeaders = pELFHeader->e_phnum;
    if (numProgHeaders == PN_XNUM)
    {
        if (pELFHeader->e_shnum == 0)
        {
            return; // Invalid ELF
        }
        Elf32_Shdr *firstSectHeader = (Elf32_Shdr *)(pPhyKernelElf + pELFHeader->e_shoff);
        numProgHeaders = firstSectHeader->sh_info;
    }

    uint8_t *alignedSegmentSpace = (uint8_t *)ALIGN_ADDRESS_CEIL(pPhyKernelElf + fileSize, PAGE_SIZE);
    for (size_t i = 0; i < numProgHeaders; i++)
    {
        Elf32_Phdr *curProgHeader = (Elf32_Phdr *)(pPhyKernelElf + pELFHeader->e_phoff + i * pELFHeader->e_phentsize);

        if (curProgHeader->p_type == PT_LOAD)
        {
            void *segmentData = pPhyKernelElf + curProgHeader->p_offset;
            memset(alignedSegmentSpace, 0, curProgHeader->p_memsz);
            memcpy(alignedSegmentSpace, segmentData, curProgHeader->p_filesz);
            paging_map((uintptr_t)alignedSegmentSpace, curProgHeader->p_memsz, curProgHeader->p_vaddr);

            alignedSegmentSpace = (uint8_t *)ALIGN_ADDRESS_CEIL(alignedSegmentSpace + curProgHeader->p_memsz, PAGE_SIZE);
        }
    }
    paging_update_table();

    jump_to_code((void *)pELFHeader->e_entry);
    __builtin_unreachable();
}
