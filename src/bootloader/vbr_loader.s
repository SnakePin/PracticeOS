[BITS 16]
;[ORG 0x7C00]

%include "defs.s"
%use ifunc ; only used for ilog2e

; BEGIN EXTERN DECLARATIONS
extern vga_clear_scr
extern vga_print_cstr
extern lba_send_transfer_packet
extern lba_extension_check
extern __vbr_size ; Defined by linker, this is not an address! It's a constant
extern vbr_second_stage
extern disable_all_interrupts
extern enable_all_interrupts
; END EXTERN DECLARATIONS

[SECTION .init_data]
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
; Stage 2 uses these variables
global FAIL_LBA_READ:data
global BOOT_DISK_ID_VAR:data
global VBR_LBA_ADDRESS_L_VAR:data
global VBR_LBA_ADDRESS_H_VAR:data
global lba_xfer_pkt:function
global print_and_halt:function
; END VARIABLE DEFINITIONS

; BEGIN CONSTANT DEFINITIONS
STACK_SEGMENT EQU 0x50     ; First free memory
STACK_SIZE EQU 0x6000      ; 24KiB
; END CONSTANT DEFINITIONS

[SECTION .init_text]
global _entry:function
_entry:
    jmp 0x0000:first_stage ; fix cs
; DS:SI = PTE in the MBR, DL = Boot Disk ID
first_stage:
    .initialize_stack:
    call disable_all_interrupts
    mov ax, STACK_SEGMENT
    mov ss, ax
    mov ax, STACK_SIZE
    mov sp, ax
    mov bp, sp
    call enable_all_interrupts
    .save_parameters:
    mov bx, word [ds:si + mp_lba_first_l]
    mov cx, word [ds:si + mp_lba_first_h]
    xor ax, ax
    mov ds, ax ; fix DS
    mov word [VBR_LBA_ADDRESS_L_VAR], bx
    mov word [VBR_LBA_ADDRESS_H_VAR], cx
    mov byte [BOOT_DISK_ID_VAR], dl
    .check_vbr_length:
    mov ax, __vbr_size
    cmp ax, LBA_SECTOR_SIZE
    jle .stage2_loaded ; If the VBR fits in the first sector, we don't have to load anything
    .load_rest_of_vbr:
    inc bx
    adc cx, 0
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], bx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], cx
    xor dx, dx
    mov bx, LBA_SECTOR_SIZE
    div bx
    test dx,dx
    jz .no_remainder
    inc ax ; Add the last sector if there is remainder
    .no_remainder:
    dec ax ; Decrease, because the first sector is already loaded
    mov word [lba_xfer_pkt + ltp_num_sector], ax
    mov word [lba_xfer_pkt + ltp_mem_offset], VBR_ADDRESS + LBA_SECTOR_SIZE
    mov word [lba_xfer_pkt + ltp_mem_segment], 0
    mov al, byte [BOOT_DISK_ID_VAR]
    mov si, lba_xfer_pkt
    call lba_send_transfer_packet
    test al, al
    mov si, FAIL_LBA_READ
    jz print_and_halt
    .stage2_loaded:
    jmp vbr_second_stage ; noreturn

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