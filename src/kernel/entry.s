[BITS 32]

extern kmain
extern load_kernel_gdt
extern load_kernel_idt
extern enable_all_interrupts

global _entry:function

[SECTION .init]
_entry:
    ; The bootloader won't set the stack up for us
    mov esp, 0x400000 ; Fourth MiB, should be conventionally free, can we move this to the memory manager?
    call load_kernel_gdt
    call load_kernel_idt
    call kmain
    .halt:
    hlt
    jmp .halt