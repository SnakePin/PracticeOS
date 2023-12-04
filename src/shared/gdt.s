[BITS 32]

global load_gdt:function

[SECTION .text]
; void load_gdt(void* gdt@ebp+8, size_t codeSegment@ebp+12, size_t dataSegment@ebp+16)
load_gdt:
    push ebp
    mov ebp, esp
    push eax
    mov eax, dword [ebp+8]
    lgdt [eax]
    mov eax, dword [ebp+16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov eax, dword [ebp+12]
    sub esp, 6
    mov dword [esp], .load_cs
    mov word [esp+4], ax
    jmp [esp] ; JMP m16:32
    .load_cs:
    add esp, 6
    pop eax
    leave
    ret
