#include <stdint.h>
#include <stddef.h>
#include "interrupt.h"
#include "utils.h"
#include "drivers/pic8259.h"

CDECL_ATTR void generic_c_isr(IRQVectorNum_t, uint32_t);
void *generate_isr_stub(IRQVectorNum_t, void const *);
extern const void *generic_asm_isr; // This variable will have the same location as our ISR, don't modify or dereference it!

/* push 0xCCCCCCCC; push 0xCCCCCCCC; ret */
const uint8_t isrTemplate[] = {0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0xC3};
#define ISR_TEMPLATE_LEN (sizeof(isrTemplate)) // I hate C
#define ISR_COUNT (256)

uint8_t SECTION_ATTR(.text.dynamic_isr #) isrArray[ISR_COUNT * ISR_TEMPLATE_LEN];
struct IDTDescriptor32_t idtDescriptor;
struct IDTEntry32_t idtArray[ISR_COUNT];

CDECL_ATTR void generic_c_isr(IRQVectorNum_t vectorNumber, uint32_t errorCode)
{
    if (IS_PIC_INTERRUPT(vectorNumber))
    {
        uint8_t picIrqNum = vectorNumber - PIC8259_IRQ_OFFSET;
        uint8_t isSpurious = pic8259_is_irq_spurious(picIrqNum);

        // TODO: do something here

        pic8259_send_eoi(picIrqNum, isSpurious);
    }
    else if (vectorNumber == SYSCALL_INTERRUPT)
    {
        // TODO: do something here
    }
}

CDECL_ATTR void load_default_idt()
{
    idtDescriptor.size = sizeof(idtArray) - 1;
    idtDescriptor.offset = (uint32_t)&idtArray;

    for (size_t i = 0; i < ISR_COUNT; i++)
    {
        void *offset = generate_isr_stub(i, &generic_asm_isr);
        idtArray[i].offset_l = (uintptr_t)offset & 0xFFFF;
        idtArray[i].offset_h = ((uintptr_t)offset >> 16) & 0xFFFF;
        idtArray[i].reserved = 0;
        idtArray[i].option_byte.raw = 0;
        idtArray[i].option_byte.Present = 1;
        idtArray[i].option_byte.GateType = Interrupt32Bit;
        idtArray[i].segment = 0x8;
    }
    
    idtArray[SYSCALL_INTERRUPT].option_byte.raw = 0;
    idtArray[SYSCALL_INTERRUPT].option_byte.Present = 1;
    idtArray[SYSCALL_INTERRUPT].option_byte.DPL = 3; //Ring 3 should be able to raise this IRQ
    idtArray[SYSCALL_INTERRUPT].option_byte.GateType = Trap32Bit;
    
    load_idt(&idtDescriptor);
}

void *generate_isr_stub(IRQVectorNum_t vectorNumber, const void *const pRealHandler)
{
    uint8_t *offset = &isrArray[vectorNumber * ISR_TEMPLATE_LEN];
    memcpy(offset, isrTemplate, ISR_TEMPLATE_LEN);
    *(uint32_t *)(&offset[1]) = vectorNumber;
    *(uint32_t *)(&offset[6]) = (uint32_t)pRealHandler;
    return (void *)offset;
}
