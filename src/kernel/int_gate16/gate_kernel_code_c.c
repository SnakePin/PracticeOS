#include "drivers/pic8259.h"
#include "int_gate16/gate.h"
#include "interrupt.h"
#include "utils.h"
#include "gdt.h"

extern const void *const __int_gate16_raw;      // Linker variable, don't modify or dereference it!
extern const void *const __int_gate16_raw_size; // Linker variable, don't modify or dereference it!

CDECL_ATTR void int_gate16_jump_to_gate(uint16_t gate16SavedEIPVarAddr, uint16_t gate16Addr, uint16_t gate16CS, uint16_t gate16DS);

void int_gate16_init()
{
    // Change this to WriteMemPhy when paging support is added
    memcpy((void *)INT_GATE16_ADDR, (void *)LINKER_VAR(__int_gate16_raw), (size_t)LINKER_VAR(__int_gate16_raw_size));
    generate_kernel_gdt((void *)INT_GATE16_GDT_ADDR, (void *)INT_GATE16_GDT_ARRAY_ADDR);
}

void int_gate16_make_call(IRQVectorNum_t vectorNumber, const INTGate16Registers_t *inRegs, INTGate16Registers_t *outRegs)
{
    // Map first MiB 1:1 if paging is on here, cause the gate will switch to real mode
    INTGate16InternalParams_t params = {.vectorNum = vectorNumber};
    params.regs = *inRegs;
    params.gdt_addr = (uint16_t)INT_GATE16_GDT_ADDR;
    params.pm_cs = GDT_KERNEL_CS;
    params.pm_ds = GDT_KERNEL_DS;
    memcpy((void *)INT_GATE16_PARAM_ADDR, (void *)&params, sizeof(INTGate16InternalParams_t));

    disable_all_interrupts();
    uint16_t oldPicMask = pic8259_get_disabled_irq_mask();
    pic8259_configure(TRUE);
    pic8259_set_disabled_irq_mask(0x0000);

    int_gate16_jump_to_gate(INT_GATE16_EIP_ADDR, INT_GATE16_ADDR, GDT_GATE16_CS, GDT_GATE16_DS);
    memcpy((void *)outRegs, (void *)INT_GATE16_PARAM_ADDR, sizeof(INTGate16Registers_t));

    //We might have to load the GDT here too if we enable paging and don't want to identity map the first MiB
    pic8259_configure(FALSE);
    pic8259_set_disabled_irq_mask(oldPicMask);
    load_kernel_idt();
    enable_all_interrupts();
}
