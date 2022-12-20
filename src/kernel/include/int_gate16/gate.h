#pragma once
#include "interrupt.h"
#include "gdt.h"

typedef struct
{
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags_ro; //Read-only
} PACKED_ATTR INTGate16Registers_t;

typedef struct
{
    INTGate16Registers_t regs;
    uint16_t gdt_addr;
    uint16_t pm_cs;
    uint16_t pm_ds;
    uint8_t vectorNum;
} PACKED_ATTR INTGate16InternalParams_t;

typedef struct
{
    uint8_t pad1[0x500];
    uint8_t gate16_code[0x2000];
    uint32_t SavedEIP;
    INTGate16InternalParams_t params;
    GDTDescriptor32_t gdtDesc;
    GDTEntry32_t gdtArray[GDT_ENTRY_COUNT];
} PACKED_ATTR FirstMiBStructure_t;

#define INT_GATE16_EBDA_RESERVED 0x80000 // This is not defined by me, our free mem ends here
#define INT_GATE16_FREE_MEM (sizeof(FirstMiBStructure_t))
#define INT_GATE16_FREE_MEM_LEN (INT_GATE16_EBDA_RESERVED - INT_GATE16_FREE_MEM)

void int_gate16_init();
void int_gate16_make_call(IRQVectorNum_t vectorNumber, const INTGate16Registers_t *inRegs, INTGate16Registers_t *outRegs);
