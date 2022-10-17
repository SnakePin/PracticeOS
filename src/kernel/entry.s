[BITS 32]

extern main
global _entry:function
[SECTION .init]
_entry:
    call main
    .halt:
    hlt
    jmp .halt