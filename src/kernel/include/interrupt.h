#pragma once
#include <stdint.h>
#include "utils.h"

enum ExceptionInterrupts {
    DivideByZeroFault = 0x00,
    DebugTrap = 0x01,
    NonMaskableInterrupt = 0x02,
    BreakpointTrap = 0x03,
    OverflowTrap = 0x04,
    BoundRangeExceededFault = 0x05,
    InvalidOpcodeFault = 0x06,
    DeviceNotAvailableFault = 0x07,
    DoubleFaultAbort = 0x08,
    CoprocessorSegmentOverrunFault = 0x09,
    InvalidTSSFault = 0x0A,
    SegmentNotPresentFault = 0x0B,
    StackSegmentFault = 0x0C,
    GeneralProtectionFault = 0x0D,
    PageFault = 0x0E,
    x87FPFault = 0x10,
    AlignmentCheckFault = 0x11,
    MachineCheckAbort = 0x12,
    SIMDFPFault = 0x13,
    VirtualizationFault = 0x14,
    ControlProtectionFault = 0x15,
    HypervisorInjectionFault = 0x1C,
    VMMCommunicationFault = 0x1D,
    SecurityFault = 0x1E
};

enum IDTGateTypes {
    TaskGate = 0x05,
    Interrupt16Bit = 0x06,
    Trap16Bit = 0x07,
    Interrupt32Bit = 0x0E,
    Trap32Bit = 0x0F
};

typedef struct
{
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t eip;
    uint32_t esp;
    uint16_t ss;
    uint16_t cs;
    uint32_t eflags;
} PACKED_ATTR InterruptSavedRegisters_t;

typedef struct {
    uint16_t offset_l;
    uint16_t segment;
    uint8_t  reserved;
    union
    {
        struct
        {
            uint8_t GateType : 4;
            uint8_t ZERO : 1;
            uint8_t DPL : 2;
            uint8_t Present : 1;
        };
        uint8_t raw;
    } options;
    uint16_t offset_h;
} PACKED_ATTR IDTEntry32_t;

typedef struct {
    uint16_t size;
    uint32_t offset;
} PACKED_ATTR IDTDescriptor32_t;

typedef uint8_t IRQVectorNum_t;

CDECL_ATTR void load_idt(void* idt);
CDECL_ATTR void load_kernel_idt();
CDECL_ATTR void disable_all_interrupts();
CDECL_ATTR void enable_all_interrupts();
void generate_kernel_idt(IDTDescriptor32_t* pDesc, IDTEntry32_t* pEntryList);
