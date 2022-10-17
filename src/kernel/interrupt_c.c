#include <stdint.h>
#include <stddef.h>
#include "interrupt.h"

// Only functions that are in or called from the assembly must specify their calling conventions
void populate_idt_array();

// TODO: Make an ISR array here with __attribute__ ((section (".text")))

struct IDTDescriptor32_t idt_descriptor;
struct IDTEntry32_t idt_array[256];

__attribute__ ((cdecl)) void generic_c_isr(uint32_t irq_nr, uint32_t error_code)
{

}

__attribute__ ((cdecl)) void load_default_idt()
{
    idt_descriptor.size = sizeof(idt_array)-1;
    idt_descriptor.offset = (uint32_t)&idt_array;
    //populate_isr_array();
    populate_idt_array();
    load_idt(&idt_descriptor);
}

void populate_idt_array()
{
    for (size_t i = 0; i < 256; i++)
    {
        //idt_array[i].offset_l = &isr_array[i] & 0xFFFF
        //idt_array[i].offset_h = (&isr_array[i]>>16) & 0xFFFF
        idt_array[i].reserved = 0;
        idt_array[i].segment = 0x8; //TODO: don't hardcode this

    }
}