[BITS 32]

extern kmain
extern load_default_gdt
extern load_default_idt
extern enable_all_interrupts

global _entry:function

[SECTION .init]
_entry:
    call load_default_gdt
    call load_default_idt
    ;call enable_all_interrupts
    call kmain
    .halt:
    hlt
    jmp .halt