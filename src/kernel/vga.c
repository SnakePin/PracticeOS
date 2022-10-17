#include <stdint.h>
#include <stddef.h>
#include "vga.h"

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