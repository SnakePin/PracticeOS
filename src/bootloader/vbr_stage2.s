[BITS 16]

%include "defs.s"
%use ifunc ; only used for ilog2e

[SECTION .data_stage2]
; BEGIN VARIABLE DEFINITIONS, STAGE2
cstring_def KERNEL_LOADED, 'Kernel loaded.'
cstring_def FAIL_KERNEL_LEN, 'Kernel length is an invalid value!'
; END VARIABLE DEFINITIONS, STAGE2

; BEGIN CONSTANT DEFINITIONS, STAGE2
DISK_TEMP_BUFFER EQU 0x6500 ; Right after stack
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
; Stage 2 trampoline
extern vbr_stage2_trampoline
; END EXTERN DECLARATIONS

; We might have to write the rest of the VBR in C if we're going to implement an FS
; But for now, we'll load the kernel from 0xFFFF offset in the partition
; Size of the kernel in sectors is a LE UInt16 at 0xFFFD, perhaps that was a bad idea?
[SECTION .text_stage2]
extern vbr_second_stage
vbr_second_stage:
    .initialize_vga:
    call vga_clear_scr
    .blind_a20_activate: ; TODO: Write proper code to do this
    mov ax, 0x2401
    int 0x15
    .read_kernel_length_from_disk:
    mov al, byte [BOOT_DISK_ID_VAR]
    mov bx, word [VBR_LBA_ADDRESS_L_VAR]
    mov cx, word [VBR_LBA_ADDRESS_H_VAR]
    push ax
    push bx
    push cx
    add bx, ADDR_TO_LBA_SECTOR(KERNEL_IMAGE_LEN_OFFSET)
    adc cx, 0
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    mov word [lba_xfer_pkt + ltp_num_sector], 1
    mov word [lba_xfer_pkt + ltp_mem_offset], DISK_TEMP_BUFFER
    mov word [lba_xfer_pkt + ltp_mem_segment], 0
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
    adc cx, 0
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    mov bx, word [DISK_TEMP_BUFFER + ADDR_TO_LBA_SECTOR_OFFSET(KERNEL_IMAGE_LEN_OFFSET)]
    test bx, bx ; If the num_sector is 0, the length is invalid
    mov si, FAIL_KERNEL_LEN
    jz print_and_halt
    mov word [lba_xfer_pkt + ltp_num_sector], bx
    mov word [lba_xfer_pkt + ltp_mem_offset], KERNEL_LOAD_OFF
    mov word [lba_xfer_pkt + ltp_mem_segment], KERNEL_LOAD_SEG
    mov si, lba_xfer_pkt
    call lba_send_transfer_packet
    test al, al
    mov si, FAIL_LBA_READ
    jz print_and_halt
    .load_kernel:
    jmp vbr_stage2_trampoline
