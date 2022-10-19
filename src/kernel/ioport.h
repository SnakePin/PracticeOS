#pragma once
#include <stdint.h>
#include "utils.h"

CDECL_ATTR void io_outx(uint16_t port, uint32_t val, uint8_t size);
CDECL_ATTR uint32_t io_inx(uint16_t port, uint8_t size);
