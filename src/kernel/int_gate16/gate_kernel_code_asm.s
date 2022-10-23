[BITS 32]

global int_gate16_jump_to_gate:function

[SECTION .text]
; void int_gate16_jump_to_gate(uint16_t gate16EIPAddr@ebp+8, uint16_t gate16Addr@ebp+12, uint16_t gate16CS@ebp+16, uint16_t gate16DS@ebp+20)
int_gate16_jump_to_gate:
    push ebp
    mov ebp, esp
    pushad
    mov dword [SAVED_ESP_VAR], esp
    lea eax, [.gate_return]
    mov dword [SAVED_EIP_VAR], eax
    ; Set *gate16EIPAddr to return_from_gate16
    lea eax, [return_from_gate16]
    mov ebx, dword [ebp+8] ; gate16EIPAddr
    mov dword [ebx], eax   ; *gate16EIPAddr
    ; Jump to 16bit code
    push dword [ebp+16] ; gate16CS
    push dword [ebp+12] ; gate16Addr
    push dword [ebp+20] ; gate16DS, operand size is dword cause "pop ds" pops a dword
    pop ds ; DS is invalid from now on until the retf, don't access it!
    retf
    .gate_return:
    popad
    leave
    ret

return_from_gate16:
    mov esp, dword [SAVED_ESP_VAR]
    push dword [SAVED_EIP_VAR]
    ret

[SECTION .data]
SAVED_ESP_VAR dd 0
SAVED_EIP_VAR dd 0
