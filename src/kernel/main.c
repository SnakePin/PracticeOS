#include <stdint.h>
#include <stddef.h>
#include "drivers/vga.h"
#include "drivers/pic8259.h"
#include "interrupt.h"

void kmain()
{
    // Interrupts are disabled on kmain() entry
    pic8259_configure(PIC8259_IRQ_OFFSET, PIC8259_IRQ_OFFSET+8);
    for (size_t i = 0; i < 16; i++)
    {
        pic8259_set_irq_status(i, 1);
    }
    enable_all_interrupts();

    // TODO: Implement a way to switch back to real mode for using BIOS interrupts
    vga_clear_scr(0x17);
    vga_print_cstr("Kernel booted!", 0x17);
    // Returning from the kernel main will halt the CPU
}
