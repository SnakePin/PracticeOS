[BITS 16]

%include "defs.s"

global vga_clear_scr
global vga_print_cstr
global lba_send_transfer_packet
global lba_extension_check

[SECTION .text]
; No return value, no arguments
vga_clear_scr:
    pusha
    push es
    mov ax, 0xB800
    mov es, ax
    xor ax, ax
    xor di, di
    mov cx, (80*25)
    rep stosw
    pop es
    popa
    ret

; No return value, cstring @ DS:SI
vga_print_cstr:
    pusha
    xor di, di
    mov dh, 0x07 ; Color attribute for all chars
    mov ax, 0xB800
    mov es, ax
    .loop:
    mov dl, byte [DS:SI]
    mov word [ES:DI], dx
    add si, 1
    add di, 2
    test dl, dl
    jnz .loop
    popa
    ret

; AL=0 indicates failure, Transfer Packet @ DS:SI, Disk ID @ al
lba_send_transfer_packet:
    pusha
    mov dl, al
    mov ah, 0x42 ; 0x42=read, 0x43=write
    int 0x13     ; transfer packet at DS:SI
    popa
    setnc al
    xor ah, ah
    ret

; AL=0 indicates failure
lba_extension_check:
    pusha
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, 0x80
    int 0x13
    popa
    setnc al
    xor ah, ah
    ret
