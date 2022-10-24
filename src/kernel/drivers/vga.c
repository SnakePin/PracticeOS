#include <stdint.h>
#include <stddef.h>
#include "drivers/vga.h"

//TODO: Remove hardcoded variables
#define VGA_COLUMN 80
#define VGA_ROW 25
#define VGA_BUFFER 0xB8000

uint16_t* volatile const vga_buffer = (uint16_t* volatile const)VGA_BUFFER;

void vga_print_cstr(uint32_t col, uint32_t row, const char* cstr, uint8_t colorAttribute) {
    size_t offset = VGA_COLUMN*row + col;
    for (size_t i = 0; cstr[i] != 0; i++)
    {
        vga_buffer[offset+i] = cstr[i] | (colorAttribute<<8);
    }
}

void vga_clear_scr(uint8_t colorAttribute) {
    for (size_t i = 0; i < VGA_COLUMN*VGA_ROW; i++)
    {
        vga_buffer[i] = (colorAttribute<<8);
    }
}