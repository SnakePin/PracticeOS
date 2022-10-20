#pragma once
#include <stdint.h>

void vga_print_cstr(const char* cstr, uint8_t colorAttribute);
void vga_clear_scr(uint8_t colorAttribute);
