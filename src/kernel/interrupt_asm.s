[BITS 32]

global disable_all_interrupts:function
global enable_all_interrupts:function
global load_idt:function

extern generic_c_isr

[SECTION .text]
disable_all_interrupts:
    cli
    push eax
    in al, 0x70
    and al, 0x7F
    out 0x70, al
    in al, 0x71
    pop eax
    ret

enable_all_interrupts:
    sti
    push eax
    in al, 0x70
    or al, 0x80
    out 0x70, al
    in al, 0x71
    pop eax
    ret

; void load_idt(void* idt@ebp+12)
load_idt:
    push eax
    push ebp
    mov ebp, esp
    mov eax, dword [ebp+12]
    lidt [eax]
    leave
    pop eax
    ret

ALIGN 4 ; Are aligned accesses really faster?
; Actual ISRs will push the IRQ vector number and jump here
generic_asm_isr:
    push ebp
    mov ebp, esp
    pushad
    mov eax, dword [ebp+4] ; vector number
    mov ecx, INTERRUPTS_WITH_ERRCODE_LEN
    xor ebx, ebx
    .loop1:
    cmp al, byte [INTERRUPTS_WITH_ERRCODE+ecx-1]
    je .has_error_code
    dec ecx
    jnz .loop1
    jmp .no_error_code
    .has_error_code:
    push dword [ebp+8] ; error code
    push eax ; interrupt number
    call generic_c_isr
    add esp, 8
    popad
    leave
    add esp, 8
    iret
    .no_error_code:
    push 0
    push eax ; interrupt number
    call generic_c_isr
    add esp, 8
    popad
    leave
    add esp, 4
    iret

[SECTION .data]
INTERRUPTS_WITH_ERRCODE db 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x11, 0x15, 0x1D, 0x1E
INTERRUPTS_WITH_ERRCODE_LEN EQU $-INTERRUPTS_WITH_ERRCODE
