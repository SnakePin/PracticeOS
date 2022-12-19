#include <stdint.h>
#include <stddef.h>
#include "interrupt.h"
#include "utils.h"

CDECL_ATTR void generic_c_isr(IRQVectorNum_t, uint32_t, InterruptSavedRegisters_t *);
static void *set_isr_handler(IRQVectorNum_t, void const *);
extern const void *const generic_asm_isr; // Linker variable, don't modify or dereference it!

#define ISR_COUNT 256

typedef struct
{
    uint8_t push_opcode1; // 0x68
    uint32_t push_operand1;
    uint8_t push_opcode2; // 0x68
    uint32_t push_operand2;
    uint8_t ret_opcode; // 0xC3
} PACKED_ATTR ISRTemplate_t;

static ISRTemplate_t SECTION_ATTR(.text.dynamic_isr #) isrArray[ISR_COUNT];
static IDTDescriptor32_t idtDescriptor;
static IDTEntry32_t idtArray[ISR_COUNT];
static InterruptHandlerFunc_t interruptHandlers[ISR_COUNT] = {0};

void install_interrupt_handler(IRQVectorNum_t vectorNumber, InterruptHandlerFunc_t handler)
{
    if (vectorNumber >= ISR_COUNT)
    {
        // TODO: log error here
        return;
    }
    interruptHandlers[vectorNumber] = handler;
}

CDECL_ATTR void generic_c_isr(IRQVectorNum_t vectorNumber, uint32_t errorCode, InterruptSavedRegisters_t *regs)
{
    if (interruptHandlers[vectorNumber] != NULL)
    {
        interruptHandlers[vectorNumber](vectorNumber, errorCode, regs);
    }
}

CDECL_ATTR void load_kernel_idt()
{
    generate_kernel_idt(&idtDescriptor, &idtArray[0]);
    load_idt(&idtDescriptor);
}

void generate_kernel_idt(IDTDescriptor32_t *pDesc, IDTEntry32_t *pEntryList)
{
    pDesc->size = sizeof(idtArray) - 1;
    pDesc->offset = (uint32_t)pEntryList;

    for (size_t i = 0; i < ISR_COUNT; i++)
    {
        void *offset = set_isr_handler(i, LINKER_VAR(generic_asm_isr));
        pEntryList[i].offset_l = (uintptr_t)offset & 0xFFFF;
        pEntryList[i].offset_h = ((uintptr_t)offset >> 16) & 0xFFFF;
        pEntryList[i].reserved = 0;
        pEntryList[i].options.raw = 0;
        pEntryList[i].options.Present = 1;
        pEntryList[i].options.GateType = Interrupt32Bit;
        pEntryList[i].segment = 0x8;
    }

    // TODO: Make traps and faults Trap32Bit
}

static void *set_isr_handler(IRQVectorNum_t vectorNumber, const void *const pRealHandler)
{
    isrArray[vectorNumber].push_opcode1 = 0x68;
    isrArray[vectorNumber].push_operand1 = vectorNumber;
    isrArray[vectorNumber].push_opcode2 = 0x68;
    isrArray[vectorNumber].push_operand2 = (uint32_t)pRealHandler;
    isrArray[vectorNumber].ret_opcode = 0xC3;
    return &isrArray[vectorNumber];
}
