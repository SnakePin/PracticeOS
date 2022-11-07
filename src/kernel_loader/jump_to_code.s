[BITS 32]

global jump_to_kernel:function

[SECTION .text]
; void jump_to_kernel(void* code@esp+4, void* newStack@esp+8, void *arg1@esp+12, void *arg2@esp+16, void *arg3@esp+20)
jump_to_kernel:
    mov ebp, esp
    mov esp, [ebp+8]
    push dword [ebp+20]
    push dword [ebp+16]
    push dword [ebp+12]
    push strict dword 0x00000000 ; There's no return
    jmp dword [ebp+4]
