[BITS 16]

%include "defs.s"

; BEGIN EXTERN DECLARATIONS
extern disable_all_interrupts
; END EXTERN DECLARATIONS

[SECTION .data_stage2]
gdt_descriptor:
    gdt_size dw GDT_SIZE-1
    gdt_offset_l dw global_descriptor_table
    gdt_offset_h dw 0x0000
global_descriptor_table:
    istruc gdt_entry_t
    iend
    istruc gdt_entry_t ; Ring 0 Code
        at gdtr_limit, dw 0xFFFF
        at gdtr_base, db 0,0,0
        at gdtr_access, db 0b1001_1010
        at gdtr_flags_and_limit17_20, db 0b1100_1111
        at gdtr_base_25_32, db 0x00
    iend
    istruc gdt_entry_t ; Ring 0 Data
        at gdtr_limit, dw 0xFFFF
        at gdtr_base, db 0,0,0
        at gdtr_access, db 0b1001_0010
        at gdtr_flags_and_limit17_20, db 0b1100_1111
        at gdtr_base_25_32, db 0x00
    iend
GDT_SIZE EQU ($ - global_descriptor_table)

[SECTION .text_stage2]
global vbr_stage2_trampoline:function
vbr_stage2_trampoline:
    call disable_all_interrupts
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:vbr_stage2_trampoline32

[BITS 32]
vbr_stage2_trampoline32:
    ; We're in 32bit protected mode here
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    mov esp, 0x00EFFFFF ; Appears to be a 14MiB guaranteed space
    hlt
    jmp KERNEL_LOAD_FLAT_ADDR
