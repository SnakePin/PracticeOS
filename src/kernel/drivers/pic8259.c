#include <stdint.h>
#include "ioport.h"
#include "drivers/pic8259.h"

/*
 * The code will follow the datasheet titled "8259A PROGRAMMABLE INTERRUPT CONTROLLER (8259A/8259A-2)" dated "December 1988"
 */

uint8_t internal_read_isr(uint8_t picNum);

// This function assumes 2 PICs present and initializes them in cascade mode
void pic8259_configure(uint8_t pic1IROffset, uint8_t pic2IROffset)
{
    /*
     * "Whenever a command is issued with A0 = 0 and D4 = 1, this is interpreted as Initialization Command
     * Word 1 (ICW1). ICW1 starts the initialization sequence during which the following automatically occur."
     */
    union ICW1Union_t icw1 = {.raw = 0};
    icw1.ONE = 1;
    icw1.IC4 = 1; // Enable ICW4
    io_outx(PIC1_PORT0, icw1.raw, 1);
    io_outx(PIC2_PORT0, icw1.raw, 1);

    // ICW2 is just the IRQ offset
    io_outx(PIC1_PORT1, pic1IROffset, 1);
    io_outx(PIC2_PORT1, pic2IROffset, 1);

    // ICW3, this one is different for master and slave PICs
    //"Slave ID is equal to the corresponding master IR input."
    io_outx(PIC1_PORT1, 0x04, 1); // Slave at IR2, this is a bit field
    io_outx(PIC2_PORT1, 0x02, 1); // Slave IDI

    union ICW4Union_t icw4 = {.raw = 0};
    icw4.microPM = 1; // 8086 mode
    io_outx(PIC1_PORT1, icw4.raw, 1);
    io_outx(PIC2_PORT1, icw4.raw, 1);
}

void pic8259_set_irq_status(uint8_t picIrqNum, uint8_t active)
{
    uint16_t port = PIC1_PORT1;
    if (picIrqNum >= 8)
    {
        port = PIC2_PORT1;
        picIrqNum -= 8;
    }

    uint8_t val = io_inx(port, 1);
    val &= ~(1 << picIrqNum);              // Clear bit
    val |= !active ? (1 << picIrqNum) : 0; // If bit is set, the IR is disabled
    io_outx(port, val, 1);
}

void pic8259_send_eoi(uint8_t picIrqNum, uint8_t isSpurious)
{
    union OCW2Union_t ocw2 = {.raw = 0};
    ocw2.EOI = 1;
    ocw2.R = 1; // Automatic priority rotation enabled

    if (picIrqNum >= 8 && !isSpurious)
    {
        // Non-spurious from slave
        io_outx(PIC2_PORT0, ocw2.raw, 1);
    }

    if (picIrqNum >= 8 || !isSpurious)
    {
        // Non-spurious from master or spurious/non-spurious from slave
        io_outx(PIC1_PORT0, ocw2.raw, 1);
    }
}

uint8_t pic8259_is_irq_spurious(uint8_t picIrqNum)
{
    uint8_t picNum = 1;
    if (picIrqNum >= 8)
    {
        picNum = 2;
        picIrqNum -= 8;
    }
    return !(internal_read_isr(picNum) & (1 << picIrqNum));
}

uint8_t internal_read_isr(uint8_t picNum)
{
    union OCW3Union_t ocw3 = {.raw = 0};
    ocw3.ONE = 1;
    ocw3.RR = 1;
    ocw3.RIS = 1;

    uint16_t port = PIC1_PORT0;
    if (picNum == 2)
    {
        port = PIC2_PORT0;
    }

    io_outx(port, ocw3.raw, 1);
    return (uint8_t)io_inx(port, 1);
}
