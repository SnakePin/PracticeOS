#include "simple_gate16/gate.h"
#include "shared/gdt.h"
#include "shared/utils.h"

CDECL_ATTR void int_gate16_jump_to_gate(uint16_t gate16CS, uint16_t gate16DS);

void int_gate16_make_call(IRQVectorNum_t vectorNumber, const INTGate16Registers_t *inRegs, INTGate16Registers_t *outRegs)
{
    INTGate16InternalParams_t params = {.vectorNum = vectorNumber};
    params.regs = *inRegs;
    params.pm_cs = GDT_KERNEL_CS;
    params.pm_ds = GDT_KERNEL_DS;
    memcpy((void *)INT_GATE16_PARAM_ADDR, (void *)&params, sizeof(INTGate16InternalParams_t));

    int_gate16_jump_to_gate(GDT_GATE16_CS, GDT_GATE16_DS);
    memcpy((void *)outRegs, (void *)INT_GATE16_PARAM_ADDR, sizeof(INTGate16Registers_t));
}
