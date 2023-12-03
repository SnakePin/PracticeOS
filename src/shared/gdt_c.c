#include "shared/gdt.h"
#include "shared/utils.h"

GDTDescriptor32_t gdtDescriptor;
GDTEntry32_t gdtArray[GDT_ENTRY_COUNT];

CDECL_ATTR void load_kernel_gdt()
{
    generate_kernel_gdt(&gdtDescriptor, &gdtArray[0]);
    load_gdt((void *)&gdtDescriptor, GDT_KERNEL_CS, GDT_KERNEL_DS);
}

void generate_kernel_gdt(GDTDescriptor32_t *pDesc, GDTEntry32_t *pEntryList)
{
    pDesc->size = GDT_ARRAY_SIZE - 1;
    pDesc->offset = (uint32_t)pEntryList;
    memset(pEntryList, 0, GDT_ARRAY_SIZE);

    /* Ring 0 Code */
    pEntryList[1].limit_l16 = 0xFFFF;
    pEntryList[1].base_l16 = 0x00;
    pEntryList[1].base_h_l8 = 0x00;
    pEntryList[1].access.raw = 0x9A; // Present, read/execute code segment
    pEntryList[1].lmt_flg.limit_h4 = 0xF;
    pEntryList[1].lmt_flg.DB = 1; // 32bit
    pEntryList[1].lmt_flg.G = 1;  // 4KiB limit granularity

    /* Ring 0 Data */
    pEntryList[2].limit_l16 = 0xFFFF;
    pEntryList[2].base_l16 = 0x00;
    pEntryList[2].base_h_l8 = 0x00;
    pEntryList[2].access.raw = 0x92; // Present, read/write data segment
    pEntryList[2].lmt_flg.limit_h4 = 0xF;
    pEntryList[2].lmt_flg.DB = 1; // 32bit
    pEntryList[2].lmt_flg.G = 1;  // 4KiB limit granularity

    /* Ring 0 16bit Code */
    pEntryList[3].limit_l16 = 0xFFFF;
    pEntryList[3].base_l16 = 0x00;
    pEntryList[3].base_h_l8 = 0x00;
    pEntryList[3].access.raw = 0x9A; // Present, read/execute code segment
    pEntryList[3].lmt_flg.limit_h4 = 0xF;
    pEntryList[3].lmt_flg.DB = 0; // 16bit
    pEntryList[3].lmt_flg.G = 0;  // byte limit granularity

    /* Ring 0 16bit Data */
    pEntryList[4].limit_l16 = 0xFFFF;
    pEntryList[4].base_l16 = 0x00;
    pEntryList[4].base_h_l8 = 0x00;
    pEntryList[4].access.raw = 0x92; // Present, read/write data segment
    pEntryList[4].lmt_flg.limit_h4 = 0xF;
    pEntryList[4].lmt_flg.DB = 0; // 16bit
    pEntryList[4].lmt_flg.G = 0;  // byte limit granularity
}
