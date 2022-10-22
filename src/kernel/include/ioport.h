#pragma once
#include <stdint.h>
#include "utils.h"

CDECL_ATTR void io_outx(uint16_t port, uint32_t val, uint8_t size);
CDECL_ATTR uint32_t io_inx(uint16_t port, uint8_t size);

//Linux-like functions
#define io_outb(x,y) io_outx(x,y,1)
#define io_outw(x,y) io_outx(x,y,2)
#define io_outl(x,y) io_outx(x,y,4)
#define io_inb(x) io_inx(x,1)
#define io_inw(x) io_inx(x,2)
#define io_inl(x) io_inx(x,4)
