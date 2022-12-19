[BITS 32]

; BEGIN DEFINITIONS
global disable_all_interrupts:function
global enable_all_interrupts:function
global load_idt:function
global generic_asm_isr:function
struc interrupt_saved_registers_t
    isreg_eax: resd 1
    isreg_ecx: resd 1
    isreg_edx: resd 1
    isreg_ebx: resd 1
    isreg_ebp: resd 1
    isreg_esi: resd 1
    isreg_edi: resd 1
    isreg_eip: resd 1
    isreg_esp: resd 1
    isreg_ss: resw 1
    isreg_cs: resw 1
    isreg_eflags: resd 1
    isreg_size:
endstruc
; END DEFINITIONS

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
    sub esp, isreg_size+1 ; isreg + uint8_t
    ; Save registers
    mov dword [ebp-isreg_size+isreg_eax], eax
    mov dword [ebp-isreg_size+isreg_ecx], ecx
    mov dword [ebp-isreg_size+isreg_edx], edx
    mov dword [ebp-isreg_size+isreg_ebx], ebx
    mov eax, dword [ebp] ; original ebp
    mov dword [ebp-isreg_size+isreg_ebp], eax
    mov dword [ebp-isreg_size+isreg_esi], esi
    mov dword [ebp-isreg_size+isreg_edi], edi
    ; Check error code availability
    xor ebx, ebx
    mov eax, dword [ebp+4] ; Vector number
    mov ecx, INTERRUPTS_WITH_ERRCODE_LEN
    .loop1:
    cmp al, byte [INTERRUPTS_WITH_ERRCODE+ecx-1]
    loopne .loop1
    sete bl
    shl bl, 2
    mov byte [ebp-isreg_size-1], bl ; 4 if interrupt has error code
    ; ebp+ebx+8 is the interrupt frame: EIP,CS,EFLAGS, ESP(?),SS(?)
    mov eax, dword [ebp+ebx+8]
    mov dword [ebp-isreg_size+isreg_eip], eax
    mov eax, dword [ebp+ebx+12]
    mov dword [ebp-isreg_size+isreg_cs], eax
    mov eax, dword [ebp+ebx+16]
    mov dword [ebp-isreg_size+isreg_eflags], eax
    ; Make the call to the ISR
    lea eax, [ebp-isreg_size]
    push eax            ; Register struct
    push dword [ebp+8]  ; Most likely going to be the EIP if there's no error code
    push dword [ebp+4]  ; Vector number
    call generic_c_isr
    ; Restore saved registers
    mov ecx, dword [ebp-isreg_size+isreg_ecx]
    mov edx, dword [ebp-isreg_size+isreg_edx]
    mov ebx, dword [ebp-isreg_size+isreg_ebx]
    mov esi, dword [ebp-isreg_size+isreg_esi]
    mov edi, dword [ebp-isreg_size+isreg_edi]
    ; Check error code availability
    mov al, byte [ebp-isreg_size-1] ; 4 if interrupt has error code
    test al, al
    mov eax, dword [ebp-isreg_size+isreg_eax]
    jnz .has_err_code
    .no_err_code:
    ; Restore EBP
    mov esp, ebp
    mov ebp, dword [ebp-isreg_size+isreg_ebp]
    add esp, 8 ; Discard the pushed EBP and the vector number
    iret
    .has_err_code:
    ; Restore EBP
    mov esp, ebp
    mov ebp, dword [ebp-isreg_size+isreg_ebp]
    add esp, 12 ; Discard the pushed EBP, vector number and the error code
    iret

[SECTION .rodata]
INTERRUPTS_WITH_ERRCODE db 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x11, 0x15, 0x1D, 0x1E
INTERRUPTS_WITH_ERRCODE_LEN EQU $-INTERRUPTS_WITH_ERRCODE
