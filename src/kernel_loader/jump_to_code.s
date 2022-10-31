[BITS 32]

global jump_to_code:function

[SECTION .text]
; void jump_to_code(void* code@esp+4)
jump_to_code:
    jmp dword [esp+4]
