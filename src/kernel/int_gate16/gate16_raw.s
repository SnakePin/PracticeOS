[ORG 0x500]
[BITS 16]

; BEGIN CONSTANT DEFINITIONS
INT_GATE16_EIP_ADDR EQU 0x24C0
INT_GATE16_PARAM_ADDR EQU 0x24C4
STACK_ADDR EQU 0x24C0 ; (8192-64) byte stack
; END CONSTANT DEFINITIONS

pm16_entry:
    ; We only have a proper 16bit CS and DS here
    mov ax, ds
    mov ss, ax
    mov word [SAVED_GATE16_DS], ax
    ; Disable protected mode
    mov eax, cr0
    mov dword [SAVED_CR0_VAR], eax
    and eax, 0x7FFFFFFE ; Disable paging and protected mode
    mov cr0, eax
    jmp 0x0000:rm16_entry ; Snap back to reality

rm16_entry:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, STACK_ADDR
    mov sp, ax
    ; Enable interrupts
    lidt [bios_default_idt]
    call enable_all_interrupts
    mov al, byte [INT_GATE16_PARAM_ADDR+g16p_vectorNum]
    mov cs:[.int_instr_modify+1], al
    ; Load registers
    mov eax, dword [INT_GATE16_PARAM_ADDR+rv32_eax]
    mov ecx, dword [INT_GATE16_PARAM_ADDR+rv32_ecx]
    mov edx, dword [INT_GATE16_PARAM_ADDR+rv32_edx]
    mov ebx, dword [INT_GATE16_PARAM_ADDR+rv32_ebx]
    mov ebp, dword [INT_GATE16_PARAM_ADDR+rv32_ebp]
    mov esi, dword [INT_GATE16_PARAM_ADDR+rv32_esi]
    mov edi, dword [INT_GATE16_PARAM_ADDR+rv32_edi]
    .int_instr_modify:
    int 0x00
    ; Store registers
    mov dword [INT_GATE16_PARAM_ADDR+rv32_eax], eax
    mov dword [INT_GATE16_PARAM_ADDR+rv32_ecx], ecx
    mov dword [INT_GATE16_PARAM_ADDR+rv32_edx], edx
    mov dword [INT_GATE16_PARAM_ADDR+rv32_ebx], ebx
    mov dword [INT_GATE16_PARAM_ADDR+rv32_ebp], ebp
    mov dword [INT_GATE16_PARAM_ADDR+rv32_esi], esi
    mov dword [INT_GATE16_PARAM_ADDR+rv32_edi], edi
    pushfd
    pop strict dword eax
    mov dword [INT_GATE16_PARAM_ADDR+rv32_eflags], eax
    jmp trampoline_to_kernel

disable_all_interrupts:
    cli
    push ax
    in al, 0x70
    and al, 0x7F
    out 0x70, al
    in al, 0x71
    pop ax
    ret

enable_all_interrupts:
    sti
    push ax
    in al, 0x70
    or al, 0x80
    out 0x70, al
    in al, 0x71
    pop ax
    ret

trampoline_to_kernel:
    call disable_all_interrupts
    mov di, word [INT_GATE16_PARAM_ADDR + g16p_gdt_addr]
    lgdt [di]
    ; Enable protected mode, will invalidate all segments, this might also enable paging
    mov bx, word [SAVED_GATE16_DS]
    mov eax, dword [SAVED_CR0_VAR]
    mov cr0, eax
    ; Set SS and DS to be able to far jump with the correct parameters
    mov ds, bx
    mov ss, bx
    ; Far jump to trampoline_to_kernel_32
    push word [INT_GATE16_PARAM_ADDR + g16p_pm_cs]
    push strict word trampoline_to_kernel_32
    retf

[BITS 32]
trampoline_to_kernel_32:
    ; We're in 32bit protected mode here
    mov ax, word [INT_GATE16_PARAM_ADDR + g16p_pm_ds]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp dword [INT_GATE16_EIP_ADDR]

; BEGIN VARIABLE DEFINITIONS
SAVED_CR0_VAR dd 0
SAVED_GATE16_DS dw 0
bios_default_idt:
    idt_size dw 0x3FF
    idt_offset_l dw 0x0000
    idt_offset_h dw 0x0000
; END VARIABLE DEFINITIONS

; BEGIN STRUCT DEFINITIONS
struc RegisterValues32_t
    rv32_eax: resd 1
    rv32_ecx: resd 1
    rv32_edx: resd 1
    rv32_ebx: resd 1
    rv32_ebp: resd 1
    rv32_esi: resd 1
    rv32_edi: resd 1
    rv32_eflags: resd 1
endstruc
struc Gate16InternalParams_t
    g16p_registerValues32: resd 8
    g16p_gdt_addr: resw 1
    g16p_pm_cs: resw 1
    g16p_pm_ds: resw 1
    g16p_vectorNum: resb 1
endstruc
; END STRUCT DEFINITIONS
