#include <stdint.h>
#include <stddef.h>
#include "drivers/vga.h"
#include "drivers/pic8259.h"
#include "int_gate16/gate.h"
#include "interrupt.h"

void kmain()
{
    // Interrupts are disabled on kmain() entry
    pic8259_configure(PIC8259_CUSTOM_IRQ_OFFSET, PIC8259_CUSTOM_IRQ_OFFSET+8);
    pic8259_set_disabled_irq_mask(0x0000); // Enable all IRs
    enable_all_interrupts();
    int_gate16_init(); //Gate for using real mode BIOS interrupts

    vga_clear_scr(0x17);
    vga_print_cstr("Kernel booted!", 0x17);
    // Returning from the kernel main will halt the CPU
}
