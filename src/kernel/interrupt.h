#pragma once
#include <stdint.h>
#include "utils.h"

CDECL_ATTR void load_idt(void* idt);
CDECL_ATTR void load_default_idt();

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
    uint8_t  option_byte;
    uint16_t offset_h;
} PACKED_ATTR;

struct IDTDescriptor32_t {
    uint16_t size;
    uint32_t offset;
} PACKED_ATTR;

