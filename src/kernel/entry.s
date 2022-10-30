[BITS 32]

extern kmain
extern load_kernel_gdt
extern load_kernel_idt
extern enable_all_interrupts

global _entry:function

[SECTION .init]
_entry:
    mov esp, 0x200000 ; Second MiB, temporary stack, allocate a real stack ASAP
    call load_kernel_gdt
    call load_kernel_idt
    call kmain
    .halt:
    hlt
    jmp .halt