[BITS 32]

global paging_load_dir:function

[SECTION .text]
; void paging_load_dir(void* pageDirectory@esp+8)
paging_load_dir:
    push eax
    mov eax, dword [esp+8]
    mov cr3, eax
    pop eax
    ret
