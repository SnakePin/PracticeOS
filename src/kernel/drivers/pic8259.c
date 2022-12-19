#include <stdint.h>
#include "ioport.h"
#include "drivers/pic8259.h"
#include "interrupt.h"

/*
 * The code will follow the datasheet
 * titled "8259A PROGRAMMABLE INTERRUPT CONTROLLER (8259A/8259A-2)" dated "December 1988"
 * and the Intel PCH datasheets.
 * As per the Intel PCH and ICH datasheets, this code will assume 2 PICs present with the slave connected to IR2 on master
 */

#define IS_VEC_PIC_INTERRUPT(x) IS_IN_RANGE(x, PIC8259_KERNEL_IRQ_OFFSET, PIC8259_KERNEL_IRQ_OFFSET + PIC8259_KERNEL_IRQ_COUNT)
static uint8_t read_isr(uint8_t picNum);

static PICInterruptHandlerFunc_t interruptHandlers[PIC8259_KERNEL_IRQ_COUNT] = {0};

void pic8259_install_interrupt_handler(PICIRQNum_t picIrqNum, PICInterruptHandlerFunc_t handler)
{
    if (picIrqNum >= PIC8259_KERNEL_IRQ_COUNT)
    {
        // TODO: log error here
        return;
    }
    interruptHandlers[picIrqNum] = handler;
}

void pic8259_interrupt_handler(IRQVectorNum_t vectorNumber, uint32_t errorCode, InterruptSavedRegisters_t *regs)
{
    if (!IS_VEC_PIC_INTERRUPT(vectorNumber))
    {
        // TODO: log error here
        return;
    }
    PICIRQNum_t picIrqNum = vectorNumber - PIC8259_KERNEL_IRQ_OFFSET;
    bool_t isSpurious = pic8259_is_irq_spurious(picIrqNum);

    if (interruptHandlers[picIrqNum] != NULL)
    {
        interruptHandlers[picIrqNum](picIrqNum);
    }

    // PIC8259 Epilogue
    pic8259_send_eoi(picIrqNum, isSpurious);
}

void pic8259_configure(bool_t isRealMode)
{
    /*
     * "Whenever a command is issued with A0 = 0 and D4 = 1, this is interpreted as Initialization Command
     * Word 1 (ICW1). ICW1 starts the initialization sequence during which the following automatically occur."
     */
    ICW1Union_t icw1 = {.raw = 0};
    icw1.ONE = 1;
    icw1.IC4 = 1; // Enable ICW4
    io_outb(PIC1_PORT0, icw1.raw);
    io_outb(PIC2_PORT0, icw1.raw);

    // ICW2 is just the IRQ offset
    if (isRealMode)
    {
        io_outb(PIC1_PORT1, PIC8259_RM_PIC1_IRQ_OFFSET);
        io_outb(PIC2_PORT1, PIC8259_RM_PIC2_IRQ_OFFSET);
    }
    else
    {
        io_outb(PIC1_PORT1, PIC8259_KERNEL_IRQ_OFFSET);
        io_outb(PIC2_PORT1, PIC8259_KERNEL_IRQ_OFFSET + 8);
    }

    // ICW3, this one is different for master and slave PICs
    //"Slave ID is equal to the corresponding master IR input."
    io_outb(PIC1_PORT1, 0x04); // Slave at IR2, this is a bit field
    io_outb(PIC2_PORT1, 0x02); // Slave IDI

    ICW4Union_t icw4 = {.raw = 0};
    icw4.microPM = 1; // 8086 mode
    io_outb(PIC1_PORT1, icw4.raw);
    io_outb(PIC2_PORT1, icw4.raw);
}

void pic8259_init()
{
    for (size_t i = PIC8259_KERNEL_IRQ_OFFSET; i <= (PIC8259_KERNEL_IRQ_OFFSET + PIC8259_KERNEL_IRQ_COUNT); i++)
    {
        install_interrupt_handler(i, pic8259_interrupt_handler);
    }
}

void pic8259_set_disabled_irq_mask(uint16_t combinedIRQMask)
{
    uint8_t masterMask = combinedIRQMask & 0xFF;
    uint8_t slaveMask = combinedIRQMask >> 8;

    io_outb(PIC1_PORT1, masterMask);
    io_outb(PIC2_PORT1, slaveMask);
}

uint16_t pic8259_get_disabled_irq_mask()
{
    uint8_t masterMask = io_inb(PIC1_PORT1);
    uint8_t slaveMask = io_inb(PIC2_PORT1);
    return masterMask | (slaveMask << 8);
}

void pic8259_send_eoi(PICIRQNum_t picIrqNum, bool_t isSpurious)
{
    OCW2Union_t ocw2 = {.raw = 0};
    ocw2.EOI = 1;
    ocw2.R = 1; // Automatic priority rotation enabled

    if (picIrqNum >= 8 && !isSpurious)
    {
        // Non-spurious from slave
        io_outb(PIC2_PORT0, ocw2.raw);
    }

    if (picIrqNum >= 8 || !isSpurious)
    {
        // Non-spurious from master or spurious/non-spurious from slave
        io_outb(PIC1_PORT0, ocw2.raw);
    }
}

bool_t pic8259_is_irq_spurious(PICIRQNum_t picIrqNum)
{
    uint8_t picNum = 1;
    if (picIrqNum >= 8)
    {
        picNum = 2;
        picIrqNum -= 8;
    }
    return !(read_isr(picNum) & (1 << picIrqNum));
}

static uint8_t read_isr(uint8_t picNum)
{
    OCW3Union_t ocw3 = {.raw = 0};
    ocw3.ONE = 1;
    ocw3.RR = 1;
    ocw3.RIS = 1;

    uint16_t port = PIC1_PORT0;
    if (picNum == 2)
    {
        port = PIC2_PORT0;
    }

    io_outb(port, ocw3.raw);
    return (uint8_t)io_inb(port);
}
