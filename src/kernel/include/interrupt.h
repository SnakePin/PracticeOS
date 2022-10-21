#pragma once
#include <stdint.h>
#include "utils.h"

typedef uint8_t IRQVectorNum_t;

#define SYSCALL_INTERRUPT (0x80)
#define PIC8259_IRQ_OFFSET (0x20)
#define PIC8259_IRQ_COUNT (16)
#define IS_PIC_INTERRUPT(x) ((x) >= PIC8259_IRQ_OFFSET && (x) < PIC8259_IRQ_OFFSET + PIC8259_IRQ_COUNT)

CDECL_ATTR void load_idt(void* idt);
CDECL_ATTR void load_default_idt();
CDECL_ATTR void disable_all_interrupts();
CDECL_ATTR void enable_all_interrupts();

enum IDTGateTypes {
    TaskGate = 0x05,
    Interrupt16Bit = 0x06,
    Trap16Bit = 0x07,
    Interrupt32Bit = 0x0E,
    Trap32Bit = 0x0F
};

struct IDTEntry32_t {
    uint16_t offset_l;
    uint16_t segment;
    uint8_t  reserved;
    union
    {
        struct
        {
            uint8_t GateType : 4;
            uint8_t ZERO : 1;
            uint8_t DPL : 2;
            uint8_t Present : 1;
        };
        uint8_t raw;
    } option_byte;
    uint16_t offset_h;
} PACKED_ATTR;

struct IDTDescriptor32_t {
    uint16_t size;
    uint32_t offset;
} PACKED_ATTR;
