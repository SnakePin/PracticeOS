#include <stdint.h>
#include <stddef.h>
#include "drivers/vga.h"
#include "drivers/pic8259.h"
#include "int_gate16/gate.h"
#include "interrupt.h"
#include "memory/memory.h"
#include "utils.h"

static char buf[32];
static char *itoa_custom(int num, int radix)
{
    const char *charset = "0123456789ABCDEF";
    if (radix > 16)
    {
        return NULL;
    }

    uint8_t negative = 0;
    size_t i = sizeof(buf) - 1;
    buf[i] = '\x00';

    if (num < 0)
    {
        negative = 1;
        num = -num;
    }
    else if(num == 0) {
        buf[--i] = '0';
    }

    while (num > 0 && i > 0)
    {
        buf[--i] = charset[num % radix];
        num /= radix;
    }

    if (negative)
    {
        buf[--i] = '-';
    }
    return &buf[i];
}

void kmain()
{
    // Interrupts are disabled on kmain() entry
    pic8259_configure(PIC8259_CUSTOM_IRQ_OFFSET, PIC8259_CUSTOM_IRQ_OFFSET + 8);
    pic8259_set_disabled_irq_mask(0x0000); // Enable all IRs
    enable_all_interrupts();
    int_gate16_init(); // Gate for using real mode BIOS interrupts
    memory_init();     // Memory functions require int_gate16

    uint32_t lineCounter = 0;
    vga_clear_scr(0x17);
    vga_print_cstr(0, lineCounter++, "Kernel is booted", 0x17);

    for (size_t currentMib = 4; currentMib <= 16; currentMib+=4)
    {
        uintptr_t pointers[4];
        for (size_t j = 0; j < 4; j++)
        {
            uint32_t columnCounter = 0;
            pointers[j] = memory_phy_allocate(1024*1024*currentMib);
            char* mib = itoa_custom(currentMib, 10);

            vga_print_cstr(columnCounter, lineCounter, mib, 0x17);
            columnCounter+=strlen(mib);

            vga_print_cstr(columnCounter, lineCounter, "MiB @ 0x", 0x17);
            columnCounter+=8;

            char* hex = itoa_custom(pointers[j], 16);
            vga_print_cstr(columnCounter, lineCounter++, hex, 0x17);
        }
        for (size_t j = 0; j < 4; j++) memory_phy_free(pointers[j]);
        vga_print_cstr(0, lineCounter++, "Freed all allocations.", 0x17);
    }

    // Returning from the kernel main will halt the CPU
}
