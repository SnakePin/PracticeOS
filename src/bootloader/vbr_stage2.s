[BITS 16]

%include "defs.s"

[SECTION .data_stage2]
; BEGIN VARIABLE DEFINITIONS, STAGE2
cstring_def VBR_LOADED, 'VBR successfully booted.'
cstring_def FAIL_KERNEL_READ, 'Unknown error while trying to read kernel from the disk!'
; END VARIABLE DEFINITIONS, STAGE2

; BEGIN CONSTANT DEFINITIONS, STAGE2
DISK_TEMP_BUFFER EQU 0x65FF ; 256 bytes past the stack
KERNEL_IMAGE_LEN_OFFSET EQU 0xFFFD
KERNEL_IMAGE_OFFSET EQU 0xFFFF
KERNEL_LOAD_SEG EQU 0x17BF ; About 64KiB past the VBR
KERNEL_LOAD_OFF EQU 0x0000
; END CONSTANT DEFINITIONS, STAGE2

; BEGIN EXTERN DECLARATIONS
extern vga_clear_scr
extern vga_print_cstr
extern lba_send_transfer_packet
extern lba_extension_check
; Stage 1 globals
extern FAIL_LBA_READ
extern BOOT_DISK_ID_VAR
extern VBR_LBA_ADDRESS_L_VAR
extern VBR_LBA_ADDRESS_H_VAR
extern lba_xfer_pkt
extern print_and_halt
; END EXTERN DECLARATIONS

; We might have to write the rest of the VBR in C if we're going to implement an FS
; But for now, we'll load the kernel from 0xFFFF offset in the partition
; Size of the kernel in sectors is a LE UInt16 at 0xFFFD, perhaps that was a bad idea?
[SECTION .text_stage2]
times 512 db 0
extern vbr_second_stage
vbr_second_stage:
    .blind_a20_activate: ; TODO: Write proper code to do this
    mov ax, 0x2401
    int 0x15
    .read_kernel_length_from_disk:
    mov word [lba_xfer_pkt + ltp_mem_segment], 0 ; flat memory
    mov al, byte [BOOT_DISK_ID_VAR]
    mov bx, word [VBR_LBA_ADDRESS_L_VAR]
    mov cx, word [VBR_LBA_ADDRESS_H_VAR]
    push ax
    push bx
    push cx
    add bx, ADDR_TO_LBA_SECTOR(KERNEL_IMAGE_LEN_OFFSET)
    setc cl ; same as adc cx, 0
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    mov word [lba_xfer_pkt + ltp_num_sector], 1
    mov word [lba_xfer_pkt + ltp_mem_offset], DISK_TEMP_BUFFER
    mov si, lba_xfer_pkt
    call lba_send_transfer_packet
    test al, al
    mov si, FAIL_LBA_READ
    jz print_and_halt
    .read_kernel:
    pop cx ; VBR_LBA_ADDRESS_H_VAR
    pop bx ; VBR_LBA_ADDRESS_L_VAR
    pop ax ; BOOT_DISK_ID_VAR
    add bx, ADDR_TO_LBA_SECTOR(KERNEL_IMAGE_OFFSET)
    setc cl ; same as adc cx, 0
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    mov bx, word [DISK_TEMP_BUFFER + ADDR_TO_LBA_SECTOR_OFFSET(KERNEL_IMAGE_LEN_OFFSET)]
    push bx
    mov word [lba_xfer_pkt + ltp_num_sector], bx
    mov word [lba_xfer_pkt + ltp_mem_offset], DISK_TEMP_BUFFER
    mov si, lba_xfer_pkt
    call lba_send_transfer_packet
    test al, al
    mov si, FAIL_LBA_READ
    jz print_and_halt
    .move_kernel:
    pop bx ; Length of the kernel in LBA sectors
    xor ax, ax
    mov ds, ax
    mov ax, KERNEL_LOAD_SEG
    mov es, ax
    mov si, DISK_TEMP_BUFFER
    add si, ADDR_TO_LBA_SECTOR_OFFSET(KERNEL_IMAGE_OFFSET)
    mov di, KERNEL_LOAD_OFF
    shl bx, 10 ; Size of kernel in words, doing this effectively limits the kernel to 64KiB
    mov cx, bx
    rep movsw
    
    mov si, VBR_LOADED
    call vga_clear_scr
    call vga_print_cstr
    jmp $