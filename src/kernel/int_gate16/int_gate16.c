#include "drivers/pic8259.h"
#include "int_gate16/gate.h"
#include "memory/memory.h"
#include "interrupt.h"
#include "shared/utils.h"

#include "shared/paging.h"
#include "shared/utils.h"

extern const void *const __int_gate16_raw;      // Linker variable, don't modify or dereference it!
extern const void *const __int_gate16_raw_size; // Linker variable, don't modify or dereference it!

CDECL_ATTR void int_gate16_jump_to_gate(uint16_t gate16SavedEIPAddr, uint16_t gate16Addr, uint16_t gate16CS, uint16_t gate16DS);

#define TO_MIB(x) ((x)*0x100000)
static FirstMiBStructure_t *firstMiBMap;

void int_gate16_init()
{
    firstMiBMap = memory_virt_create_phy_view(0, TO_MIB(1));

    // Change this to WriteMemPhy when paging support is added
    memcpy(&firstMiBMap->gate16_code[0], (void *)LINKER_VAR(__int_gate16_raw), (size_t)LINKER_VAR(__int_gate16_raw_size));
    generate_kernel_gdt(&firstMiBMap->gdtDesc, &firstMiBMap->gdtArray[0]);
    // Descriptor fixup for the real mode
    firstMiBMap->gdtDesc.offset -= (uintptr_t)firstMiBMap;
}

void int_gate16_make_call(IRQVectorNum_t vectorNumber, const INTGate16Registers_t *inRegs, INTGate16Registers_t *outRegs)
{
    firstMiBMap->params.vectorNum = vectorNumber;
    firstMiBMap->params.regs = *inRegs;
    firstMiBMap->params.gdt_addr = offsetof(FirstMiBStructure_t, gdtDesc);
    firstMiBMap->params.pm_cs = GDT_KERNEL_CS;
    firstMiBMap->params.pm_ds = GDT_KERNEL_DS;

    disable_all_interrupts();
    uint16_t oldPicMask = pic8259_get_disabled_irq_mask();
    pic8259_configure(TRUE);
    pic8259_set_disabled_irq_mask(0x0000);

    // Identity map the first MiB, cause the gate will switch to real mode
    size_t bufferSize = LEN_TO_PAGE(TO_MIB(1)) * sizeof(PageTableEntry_t);
    PageTableEntry_t *savedPagesBuffer = memory_virt_allocate(bufferSize);
    paging_save_tables(0, TO_MIB(1), savedPagesBuffer, bufferSize);
    paging_map(0, (void *)0, TO_MIB(1));
    int_gate16_jump_to_gate(offsetof(FirstMiBStructure_t, SavedEIP), offsetof(FirstMiBStructure_t, gate16_code), GDT_GATE16_CS, GDT_GATE16_DS);
    paging_restore_tables(0, TO_MIB(1), savedPagesBuffer);
    memory_virt_free(savedPagesBuffer, bufferSize);

    *outRegs = firstMiBMap->params.regs;
    
    pic8259_configure(FALSE);
    pic8259_set_disabled_irq_mask(oldPicMask);
    load_kernel_gdt(); //gate16 loads it's own GDT when switching back to protected mode
    load_kernel_idt();
    enable_all_interrupts();
}
