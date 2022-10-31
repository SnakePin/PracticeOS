[BITS 16]
;[ORG 0x7C00]

%include "defs.s"
%use ifunc ; only used for ilog2e

; BEGIN EXTERN/GLOBAL DECLARATIONS
extern vga_clear_scr
extern vga_print_cstr
extern lba_send_transfer_packet
extern vbr_trampoline16
extern disable_all_interrupts
extern enable_all_interrupts
global BOOT_DISK_ID_VAR
global BOOT_PAR_LBA_L_VAR
global BOOT_PAR_LBA_H_VAR
; END EXTERN/GLOBAL DECLARATIONS

[SECTION .data]
; BEGIN VARIABLE DEFINITIONS
cstring_def UNSPECIFIED_FAIL, 'VBR failed!'
BOOT_DISK_ID_VAR db 0x00
BOOT_PAR_LBA_L_VAR dw 0x0000 ; Our LBA address
BOOT_PAR_LBA_H_VAR dw 0x0000
align 4 ; LBA Transfer Packet must be aligned to 4 bytes
lba_xfer_pkt:
    istruc lba_transfer_packet_t 
        at ltp_size, db 16 
        at ltp_reserved, db 0
    iend

; BEGIN CONSTANT DEFINITIONS
STACK_SEGMENT EQU 0x70     ; Stack ends right after the MBR copy
STACK_SIZE EQU 0x6000      ; 24KiB
DISK_TEMP_BUFFER EQU 0x6700 ; Right after stack
; END CONSTANT DEFINITIONS

[SECTION .init]
global _entry:function
_entry:
    jmp 0x0000:vbr_main ; fix cs

[SECTION .text]
; DS:SI = PTE in the MBR, DL = Boot Disk ID
vbr_main:
    ; Initialize a proper stack
    call disable_all_interrupts
    mov ax, STACK_SEGMENT
    mov ss, ax
    mov ax, STACK_SIZE
    mov sp, ax
    mov bp, sp
    call enable_all_interrupts
    ; Save parameters passed from the MBR
    mov bx, word [ds:si + mp_lba_first_l]
    mov cx, word [ds:si + mp_lba_first_h]
    xor ax, ax
    mov ds, ax ; fix DS
    mov word [BOOT_PAR_LBA_L_VAR], bx
    mov word [BOOT_PAR_LBA_H_VAR], cx
    mov byte [BOOT_DISK_ID_VAR], dl
    ; Enable A20 line, TODO: Write proper code to do this
    mov ax, 0x2401
    int 0x15
    ; Read length word from disk
    mov word [lba_xfer_pkt + ltp_mem_offset], DISK_TEMP_BUFFER
    mov word [lba_xfer_pkt + ltp_mem_segment], 0
    mov bx, ADDR_TO_LBA_SECTOR(KERNEL_LDR_IMG_LEN_OFF)
    xor cx, cx
    mov ax, 1 ; num_sector
    call vbr_read_from_boot_par
    test al, al
    jz vbr_error
    ; Read kernel from disk
    mov word [lba_xfer_pkt + ltp_mem_offset], KERNEL_LDR_LOAD_OFF
    mov word [lba_xfer_pkt + ltp_mem_segment], KERNEL_LDR_LOAD_SEG
    mov bx, ADDR_TO_LBA_SECTOR(KERNEL_LDR_IMG_OFFSET)
    xor cx, cx
    mov ax, word [DISK_TEMP_BUFFER + ADDR_TO_LBA_SECTOR_OFFSET(KERNEL_LDR_IMG_LEN_OFF)] ; num_sector
    call vbr_read_from_boot_par
    test al, al
    jz vbr_error
    ; Jump to the trampoline
    jmp vbr_trampoline16

; Offset @ CX:BX, num_sector @ AX, ltp_mem_segment and ltp_mem_offset must be set manually
vbr_read_from_boot_par:
    test ax, ax ; If the num_sector is 0, the length is invalid
    jz vbr_error
    mov word [lba_xfer_pkt + ltp_num_sector], ax
    mov al, byte [BOOT_DISK_ID_VAR]
    add bx, word [BOOT_PAR_LBA_L_VAR]
    adc cx, 0
    add cx, word [BOOT_PAR_LBA_H_VAR]
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    mov si, lba_xfer_pkt
    call lba_send_transfer_packet
    ret

vbr_error:
    mov si, UNSPECIFIED_FAIL
    call vga_clear_scr
    call vga_print_cstr
    .halt: ; Maybe shutdown here instead?
    cli
    hlt
    jmp .halt
