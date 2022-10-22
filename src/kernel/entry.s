[BITS 32]

extern kmain
extern load_kernel_gdt
extern load_kernel_idt
extern enable_all_interrupts

global _entry:function

[SECTION .init]
_entry:
    call load_kernel_gdt
    call load_kernel_idt
    call kmain
    .halt:
    hlt
    jmp .halt