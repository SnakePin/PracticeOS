#include <stdint.h>
#include <stddef.h>
#include "vga.h"

void kmain()
{
    //TODO: implement interrupts and a way to switch back to real mode for using BIOS interrupts
    vga_clear_scr(0x17);
    vga_print_cstr("Kernel booted!", 0x17);
    //Returning from the kernel main will halt the CPU
}
