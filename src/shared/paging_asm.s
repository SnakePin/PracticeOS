[BITS 32]

global paging_load_dir:function
global paging_enable_paging:function

[SECTION .text]
; void paging_load_dir(void* pageDirectory@esp+8)
paging_load_dir:
    push eax
    mov eax, dword [esp+8]
    mov cr3, eax
    pop eax
    ret

; void paging_enable_paging()
paging_enable_paging:
    push eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    pop eax
    ret
