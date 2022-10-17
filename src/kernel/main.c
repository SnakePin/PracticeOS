#include <stdint.h>
#include <stddef.h>

#define VGA_COLUMN 80
#define VGA_ROW 25
#define VGA_BUFFER 0xB8000

uint16_t* volatile const vga_buffer = (uint16_t* volatile const)VGA_BUFFER;

void vga_print_cstr(const char* cstr, uint8_t colorAttribute) {
    for (size_t i = 0; cstr[i] != 0; i++)
    {
        vga_buffer[i] = cstr[i] | (colorAttribute<<8);
    }
}

void vga_clear_scr(uint8_t colorAttribute) {
    //TODO: remove hardcoded variables
    //Assuming 80*25
    for (size_t i = 0; i < VGA_COLUMN*VGA_ROW; i++)
    {
        vga_buffer[i] = (colorAttribute<<8);
    }
}

void main()
{
    //TODO: implement interrupts and a way to switch back to real mode for using BIOS interrupts
    vga_clear_scr(0x17);
    vga_print_cstr("Kernel booted!", 0x17);
    //Returning from the kernel main will halt the CPU
}
