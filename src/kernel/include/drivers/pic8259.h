#pragma once
#include <stdint.h>
#include "utils.h"

void pic8259_configure(uint8_t pic1IROffset, uint8_t pic2IROffset);
uint16_t pic8259_get_disabled_irq_mask();
void pic8259_set_disabled_irq_mask(uint16_t combinedIRQMask);
void pic8259_send_eoi(uint8_t picIrqNum, uint8_t isSpurious);
uint8_t pic8259_is_irq_spurious(uint8_t picIrqNum);

/*
 * A0 in the datasheet refers to the last bit of the port address.
 * PCH datasheets show that the chipset emulates two 8259s at 0x20 and 0xA0 respectively
 */
#define PIC1_PORT0 (0x20)
#define PIC1_PORT1 (0x21)
#define PIC2_PORT0 (0xA0)
#define PIC2_PORT1 (0xA1)

#define PIC8259_RM_PIC1_IRQ_OFFSET (0x08)
#define PIC8259_RM_PIC2_IRQ_OFFSET (0x70)

#define PIC8259_CUSTOM_IRQ_COUNT (16)
#define PIC8259_CUSTOM_IRQ_OFFSET (0x20)
#define IS_VEC_PIC_INTERRUPT(x) IS_IN_RANGE(x, PIC8259_CUSTOM_IRQ_OFFSET, PIC8259_CUSTOM_IRQ_OFFSET+PIC8259_CUSTOM_IRQ_COUNT)

// Bit field structs assume an LE system
typedef union
{
    struct
    {
        uint8_t IC4 : 1; // 1 for ICW4, 0 for no ICW4
        uint8_t SNGL : 1; // 0 for cascade mode, 1 for single mode
        uint8_t ADI : 1;
        uint8_t LTIM : 1;
        uint8_t ONE : 1; // MUST be set to 1
        uint8_t ZERO : 3;
    } PACKED_ATTR;
    uint8_t raw;
} ICW1Union_t;
typedef union
{
    struct
    {
        uint8_t microPM : 1; // 1 for 8086, 0 for MCS-80/85. Should be 1 for all modern systems.
        uint8_t AEOI : 1; // auto end of interrupt
        uint8_t BUF : 1; // 1 for buffered mode
        uint8_t BUFMS : 1; // 1 for master, 0 for slave
        uint8_t SFNM : 1; // special fully nested mode
        uint8_t ZERO : 3;
    } PACKED_ATTR;
    uint8_t raw;
} ICW4Union_t;
typedef union
{
    struct
    {
        uint8_t IRLevel : 3;
        uint8_t ZERO : 2;
        uint8_t EOI : 1;
        uint8_t SL : 1;
        uint8_t R : 1;
    } PACKED_ATTR;
    uint8_t raw;
} OCW2Union_t;
typedef union
{
    struct
    {
        uint8_t RIS : 1;
        uint8_t RR : 1;
        uint8_t P : 1;
        uint8_t ONE : 1;
        uint8_t ZERO : 1;
        uint8_t SMM : 1;  // New value of SM register
        uint8_t ESMM : 1; // 1 for changing SM register
        uint8_t ZERO1 : 1;
    } PACKED_ATTR;
    uint8_t raw;
} OCW3Union_t;
