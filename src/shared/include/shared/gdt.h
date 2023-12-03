#pragma once
#include <stdint.h>
#include "shared/utils.h"

#define GDT_ENTRY_COUNT 5
#define GDT_ARRAY_SIZE (sizeof(GDTEntry32_t) * GDT_ENTRY_COUNT)
#define GDT_KERNEL_CS (1*8)
#define GDT_KERNEL_DS (2*8)
#define GDT_GATE16_CS (3*8)
#define GDT_GATE16_DS (4*8)

typedef struct
{
    uint16_t limit_l16;
    uint16_t base_l16;
    uint8_t base_h_l8;
    union
    {
        struct
        {
            uint8_t A : 1; // Accessed
            uint8_t RW : 1;
            uint8_t DC : 1;
            uint8_t E : 1; // 1 for code, 0 for data
            uint8_t S : 1; // 1 for code/data, 0 for system segments
            uint8_t DPL : 2;
            uint8_t P : 1; // Present bit
        } PACKED_ATTR;
        uint8_t raw;
    } access;
    union
    {
        struct
        {
            uint8_t limit_h4 : 4;
            uint8_t unused : 1;
            uint8_t L : 1;  // Is long mode segment
            uint8_t DB : 1; // Is 32bit segment
            uint8_t G : 1;  // 1 for 4KiB, 0 for byte limit granularity
        } PACKED_ATTR;
        uint8_t raw;
    } lmt_flg;
    uint8_t base_h_h8;
} PACKED_ATTR GDTEntry32_t;

typedef struct
{
    uint16_t size;
    uint32_t offset;
} PACKED_ATTR GDTDescriptor32_t;

CDECL_ATTR void load_gdt(void *pGDT, uint16_t codeSegment, uint16_t dataSegment);
CDECL_ATTR void load_kernel_gdt();
void generate_kernel_gdt(GDTDescriptor32_t *pDesc, GDTEntry32_t *pEntryList);
