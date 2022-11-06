[BITS 32]

global jump_to_code:function

[SECTION .text]
; void jump_to_code(void* code@esp+4, ...)
jump_to_code:
    add esp, 4 ; We must pass the arguments as-is
    jmp dword [esp]
