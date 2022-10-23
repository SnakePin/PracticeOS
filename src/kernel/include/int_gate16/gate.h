#pragma once
#include "interrupt.h"
#include "gdt.h"

#define INT_GATE16_ADDR 0x500
#define INT_GATE16_EIP_ADDR 0x24C0
#define INT_GATE16_PARAM_ADDR 0x24C4
#define INT_GATE16_GDT_ADDR (INT_GATE16_PARAM_ADDR+sizeof(INTGate16InternalParams_t))
#define INT_GATE16_GDT_ARRAY_ADDR (INT_GATE16_GDT_ADDR+sizeof(GDTDescriptor32_t))
#define INT_GATE16_FIRST_FREE_MEM (INT_GATE16_GDT_ARRAY_ADDR+GDT_ARRAY_SIZE)

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

void int_gate16_init();
void int_gate16_make_call(IRQVectorNum_t vectorNumber, const INTGate16Registers_t *inRegs, INTGate16Registers_t *outRegs);
