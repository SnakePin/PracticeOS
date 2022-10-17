#pragma once
#include <stdint.h>
__attribute__ ((cdecl)) void load_idt(void* idt);
__attribute__ ((cdecl)) void generic_c_isr(uint32_t irq_nr, uint32_t error_code);
__attribute__ ((cdecl)) void load_default_idt();

struct IDTEntry32_t {
    uint16_t offset_l;
    uint16_t segment;
    uint8_t  reserved;
    uint8_t  option_byte;
    uint16_t offset_h;
} __attribute__((packed));

struct IDTDescriptor32_t {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

