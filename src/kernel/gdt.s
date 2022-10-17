[BITS 32]

global load_gdt:function
global load_default_gdt:function

struc gdt_entry_t
    gdte_limit: resw 1
    gdte_base: resb 3
    gdte_access: resb 1
    gdte_flags_and_limit17_20: resb 1
    gdte_base_25_32: resb 1
endstruc

[SECTION .text]
; void load_gdt(void* gdt@ebp+12, size_t codeSegment@ebp+16, size_t dataSegment@ebp+20)
load_gdt:
    push eax
    push ebp
    mov ebp, esp
    mov eax, dword [ebp+12]
    lgdt [eax]
    mov eax, dword [ebp+20]
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    mov eax, dword [ebp+16]
    sub esp, 6
    mov dword [esp], .load_cs
    mov word [esp+4], ax
    jmp [esp] ; JMP m16:32
    .load_cs:
    leave
    pop eax
    ret

load_default_gdt:
    push 0x10 ; Ring 0 Data
    push 0x08 ; Ring 0 Code
    push gdt_descriptor
    call load_gdt
    add esp, 12
    ret

[SECTION .data]
;TODO: add a TSS for ISRs and a kernel stack allocator
;Maybe generate the GDT in C too?
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
    istruc gdt_entry_t ; Ring 0 16Bit Code
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_1010
        at gdte_flags_and_limit17_20, db 0b0000_1111
        at gdte_base_25_32, db 0x00
    iend
GDT_SIZE EQU ($ - global_descriptor_table)