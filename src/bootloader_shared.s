; BEGIN CONSTANT DEFINITIONS
MBR_PT_OFFSET EQU 446      ; MBR partition table offset
MBR_PTE_SIZE EQU 16        ; MBR partition table entry size
; END CONSTANT DEFINITIONS

; BEGIN STRUCT DEFINITIONS
struc lba_transfer_packet_t
    ltp_size: resb 1
    ltp_reserved: resb 1
    ltp_num_sector: resw 1
    ltp_mem_offset: resw 1
    ltp_mem_segment: resw 1
    ltp_lba_addr_lower32_l: resw 1
    ltp_lba_addr_lower32_h: resw 1
    ltp_lba_addr_higher32_l: resw 1
    ltp_lba_addr_higher32_h: resw 1
endstruc
struc mbr_pte_t
    mp_status: resb 1
    mp_CHSFirst: resb 3
    mp_type: resb 1
    mp_CHSLast: resb 3
    mp_lba_first_l: resw 1
    mp_lba_first_h: resw 1
    mp_sector_count_l: resw 1
    mp_sector_count_h: resw 1
endstruc
; END STRUCT DEFINITIONS

; No return value, no arguments
clear_scr_vga:
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
print_cstr_vga:
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

%macro cstring_def 2
    %1 db %2, 0x00
    %1_LEN EQU $-%1
%endmacro
