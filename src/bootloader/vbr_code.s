[BITS 16]
;[ORG 0x7C00]

%include "defs.s"

; BEGIN EXTERN DECLARATIONS
extern vga_clear_scr
extern vga_print_cstr
extern lba_send_transfer_packet
extern lba_extension_check
extern __vbr_actual_size$ ; Defined by linker
; END EXTERN DECLARATIONS

[SECTION .data]
; BEGIN VARIABLE DEFINITIONS
cstring_def FAIL_LBA_READ, 'LBA read fail!'
BOOT_DISK_ID_VAR db 0x00
VBR_LBA_ADDRESS_L_VAR dw 0x0000 ; Our LBA address
VBR_LBA_ADDRESS_H_VAR dw 0x0000
align 4 ; LBA Transfer Packet must be aligned to 4 bytes
lba_xfer_pkt:
    istruc lba_transfer_packet_t 
        at ltp_size, db 16 
        at ltp_reserved, db 0
    iend
; END VARIABLE DEFINITIONS

; BEGIN CONSTANT DEFINITIONS
VBR_SECTORS_TO_LOAD EQU LEN_TO_LBA_SECTOR(VBR_ACTUAL_SIZE)-1 ; -1 cause the first sector is already loaded
STACK_SEGMENT EQU 0x50     ; First free memory
STACK_SIZE EQU 0x6000      ; 24KiB
; END CONSTANT DEFINITIONS

[SECTION .text]
global _entry
_entry:
    jmp 0x0000:first_stage ; fix cs
; DS:SI = PTE in the MBR, DL = Boot Disk ID
first_stage:
    .initialize_stack:
    cli ; Maybe disable NMIs here too?
    mov ax, STACK_SEGMENT
    mov ss, ax
    mov ax, STACK_SIZE
    mov sp, ax
    mov bp, sp
    sti
    .save_parameters:
    mov bx, word [ds:si + mp_lba_first_l]
    mov cx, word [ds:si + mp_lba_first_h]
    xor ax, ax
    mov ds, ax ; fix DS
    mov word [VBR_LBA_ADDRESS_L_VAR], bx
    mov word [VBR_LBA_ADDRESS_H_VAR], cx
    mov byte [BOOT_DISK_ID_VAR], dl
%if VBR_SECTORS_TO_LOAD > 0
    .load_rest_of_vbr:
    inc bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    mov word [lba_xfer_pkt + ltp_num_sector], VBR_SECTORS_TO_LOAD
    mov word [lba_xfer_pkt + ltp_mem_offset], VBR_ADDRESS + LBA_SECTOR_SIZE
    mov word [lba_xfer_pkt + ltp_mem_segment], 0
    mov si, lba_xfer_pkt
    mov al, dl ; DL is already set here
    call lba_send_transfer_packet
    test al, al
    mov si, FAIL_LBA_READ
    jz print_and_halt
%endif
    jmp second_stage ; noreturn

; String at DS:SI
print_and_halt:
    call vga_clear_scr
    call vga_print_cstr
    .halt:
    cli
    hlt
    jmp .halt ; An NMI can bring the IP here

; Assert, because the loader code must fit in the first sector
%if ($ - $$) > 512
%error "VBR First stage must be within the first 512 bytes"
%endif

[SECTION .data2]
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

; We might have to write the rest of the VBR in C if we're going to implement an FS
; But for now, we'll load the kernel from 0xFFFF offset in the partition
; Size of the kernel in sectors is a LE UInt16 at 0xFFFD, perhaps that was a bad idea?
[SECTION .text2]
second_stage:
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

VBR_ACTUAL_SIZE EQU ($ - $$)
%if VBR_ACTUAL_SIZE > 65536
%error "VBR code is larger than 64KiB"
%endif