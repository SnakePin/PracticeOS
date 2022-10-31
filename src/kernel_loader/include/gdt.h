#pragma once
#include <stdint.h>
#include "utils.h"

#define GDT_ENTRY_COUNT 5
#define GDT_KERNEL_CS (1*8)
#define GDT_KERNEL_DS (2*8)
#define GDT_GATE16_CS (3*8)
#define GDT_GATE16_DS (4*8)

CDECL_ATTR void load_default_gdt();
