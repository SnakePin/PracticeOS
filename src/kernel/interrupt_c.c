#include <stdint.h>
#include <stddef.h>
#include "interrupt.h"
#include "utils.h"
#include "drivers/pic8259.h"

CDECL_ATTR void generic_c_isr(IRQVectorNum_t, uint32_t);
static void *generate_isr_stub(IRQVectorNum_t, void const *);
extern const void *const generic_asm_isr; // Linker variable, don't modify or dereference it!

/* push 0xCCCCCCCC; push 0xCCCCCCCC; ret */
const uint8_t isrTemplate[] = {0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0xC3};
#define ISR_TEMPLATE_LEN (sizeof(isrTemplate)) // I hate C
#define ISR_COUNT (256)
#define IDT_ARRAY_SIZE (sizeof(IDTEntry32_t)*ISR_COUNT)

uint8_t SECTION_ATTR(.text.dynamic_isr#) isrArray[ISR_COUNT * ISR_TEMPLATE_LEN];
IDTDescriptor32_t idtDescriptor;
IDTEntry32_t idtArray[ISR_COUNT];

CDECL_ATTR void generic_c_isr(IRQVectorNum_t vectorNumber, uint32_t errorCode)
{
    if (IS_VEC_PIC_INTERRUPT(vectorNumber))
    {
        uint8_t picIrqNum = vectorNumber - PIC8259_CUSTOM_IRQ_OFFSET;
        bool_t isSpurious = pic8259_is_irq_spurious(picIrqNum);

        // TODO: do something here

        pic8259_send_eoi(picIrqNum, isSpurious);
    }
    else if (vectorNumber == SYSCALL_INTERRUPT)
    {
        // TODO: do something here
    }
}

CDECL_ATTR void load_kernel_idt()
{
    generate_kernel_idt(&idtDescriptor, &idtArray[0]);
    load_idt(&idtDescriptor);
}

void generate_kernel_idt(IDTDescriptor32_t* pDesc, IDTEntry32_t* pEntryList)
{
    pDesc->size = IDT_ARRAY_SIZE - 1;
    pDesc->offset = (uint32_t)pEntryList;

    for (size_t i = 0; i < ISR_COUNT; i++)
    {
        void *offset = generate_isr_stub(i, LINKER_VAR(generic_asm_isr));
        pEntryList[i].offset_l = (uintptr_t)offset & 0xFFFF;
        pEntryList[i].offset_h = ((uintptr_t)offset >> 16) & 0xFFFF;
        pEntryList[i].reserved = 0;
        pEntryList[i].options.raw = 0;
        pEntryList[i].options.Present = 1;
        pEntryList[i].options.GateType = Interrupt32Bit;
        pEntryList[i].segment = 0x8;
    }
    
    pEntryList[SYSCALL_INTERRUPT].options.DPL = 3; // Ring 3 should be able to raise this IRQ
    pEntryList[SYSCALL_INTERRUPT].options.GateType = Trap32Bit;
    //TODO: Make traps and faults Trap32Bit
}

static void *generate_isr_stub(IRQVectorNum_t vectorNumber, const void *const pRealHandler)
{
    uint8_t *offset = &isrArray[vectorNumber * ISR_TEMPLATE_LEN];
    memcpy(offset, isrTemplate, ISR_TEMPLATE_LEN);
    *(uint32_t *)(&offset[1]) = vectorNumber;
    *(uint32_t *)(&offset[6]) = (uint32_t)pRealHandler;
    return (void *)offset;
}
