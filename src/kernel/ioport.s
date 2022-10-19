[BITS 32]

global io_outx:function
global io_inx:function

[SECTION .text]
; void io_outx(uint16_t port@ebp+8, uint32_t val@ebp+12, uint8_t size@ebp+16)
io_outx:
    push ebp
    mov ebp, esp
    push edx
    push eax
    push ecx
    mov edx, word [ebp+8]
    mov eax, dword [ebp+12]
    mov ecx, byte [ebp+16]
    cmp ecx, 1
    je .1byte
    cmp ecx, 2
    je .2byte
    .4byte:
    out dx, eax
    jmp .finish
    .2byte:
    out dx, ax
    jmp .finish
    .1byte:
    out dx, al
    .finish:
    pop ecx
    pop eax
    pop edx
    leave
    ret

; uint32_t io_inx(uint16_t port@ebp+8, uint8_t size@ebp+12)
io_inx:
    push ebp
    mov ebp, esp
    push edx
    mov edx, word [ebp+8]
    mov eax, byte [ebp+12]
    cmp eax, 1
    je .1byte
    cmp eax, 2
    je .2byte
    .4byte:
    in eax, dx
    jmp .finish
    .2byte:
    in ax, dx
    jmp .finish
    .1byte:
    in al, dx
    .finish:
    pop edx
    leave
    ret
