[BITS 32]

; BEGIN DEFINITIONS
global load_default_gdt:function
struc gdt_entry_t
    gdte_limit: resw 1
    gdte_base: resb 3
    gdte_access: resb 1
    gdte_flags_and_limit17_20: resb 1
    gdte_base_25_32: resb 1
endstruc
; END DEFINITIONS

[SECTION .data]
gdt_descriptor:
    gdt_size dw GDT_SIZE-1
    gdt_offset dd global_descriptor_table
global_descriptor_table:
    istruc gdt_entry_t
    iend
    istruc gdt_entry_t ; Ring 0 Code
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_1010
        at gdte_flags_and_limit17_20, db 0b1100_1111
        at gdte_base_25_32, db 0x00
    iend
    istruc gdt_entry_t ; Ring 0 Data
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_0010
        at gdte_flags_and_limit17_20, db 0b1100_1111
        at gdte_base_25_32, db 0x00
    iend
    istruc gdt_entry_t ; Ring 0 16bit Code
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_1010
        at gdte_flags_and_limit17_20, db 0b0000_1111
        at gdte_base_25_32, db 0x00
    iend
    istruc gdt_entry_t ; Ring 0 16bit Data
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_0010
        at gdte_flags_and_limit17_20, db 0b0000_1111
        at gdte_base_25_32, db 0x00
    iend
GDT_SIZE EQU ($ - global_descriptor_table)

[SECTION .text]
; void load_default_gdt()
load_default_gdt:
    push eax
    lgdt [gdt_descriptor]
    mov eax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.load_cs
    .load_cs:
    pop eax
    ret
