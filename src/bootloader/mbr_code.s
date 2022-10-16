[BITS 16]
;[ORG 0]
; nasm doesn't support changing base mid-program, so we'll set it to 0 and set segment registers accordingly

%include "defs.s"

; BEGIN EXTERN DECLARATIONS
extern vga_clear_scr
extern vga_print_cstr
extern lba_send_transfer_packet
extern lba_extension_check
extern disable_all_interrupts
extern enable_all_interrupts
; END EXTERN DECLARATIONS

[SECTION .data]
; BEGIN VARIABLE DEFINITIONS
cstring_def BOOTING_VBR_MSG, 'Booting VBR...'
cstring_def FAIL_NO_LBA_EXTENSION, 'System has no LBA support!'
cstring_def FAIL_LBA_READ, 'LBA read fail in MBR!'
cstring_def FAIL_NO_BOOTABLE_FOUND, 'No active partition found!'
BOOT_DISK_ID_VAR db 0x00
MBR_BOOT_PTE_VAR dw 0x0000
align 4 ; LBA Transfer Packet must be aligned to 4 bytes
lba_xfer_pkt:
    istruc lba_transfer_packet_t 
        at ltp_size, db 16 
        at ltp_reserved, db 0
    iend
; END VARIABLE DEFINITIONS

; BEGIN CONSTANT DEFINITIONS
MBR_COPY_SEGMENT EQU 0x50  ; First free memory address, right after BDA
MBR_ORIG_SEGMENT EQU 0x7C0 ; Original MBR segment
STACK_SEGMENT EQU 0x70     ; 512bytes past the MBR copy
STACK_SIZE EQU 0x6000      ; 24KiB
; END CONSTANT DEFINITIONS

[SECTION .init_text]
global _entry:function
_entry:
    jmp MBR_ORIG_SEGMENT:first_stage ; fix cs
first_stage:
    .initialize_stack:
    call disable_all_interrupts
    mov ax, STACK_SEGMENT
    mov ss, ax
    mov ax, STACK_SIZE
    mov sp, ax
    mov bp, sp
    call enable_all_interrupts
    .copy_mbr:
    xor si, si
    xor di, di
    mov ax, MBR_ORIG_SEGMENT
    mov ds, ax
    mov ax, MBR_COPY_SEGMENT
    mov es, ax
    mov cx, (MBR_SIZE / 2)
    rep movsw ; DS:SI to ES:DI, cx times, 2 steps at once
    jmp MBR_COPY_SEGMENT:second_stage

second_stage:
    mov ax, MBR_COPY_SEGMENT
    mov ds, ax ; Both the VGA and the LBA functions read from DS:SI
    mov byte [BOOT_DISK_ID_VAR], dl
    .lba_check:
    call lba_extension_check
    test al, al
    mov si, FAIL_NO_LBA_EXTENSION
    jz .error
    .lba_check_success:
    call mbr_get_vbr_lba
    mov word [MBR_BOOT_PTE_VAR], bx
    test al, al
    mov si, FAIL_NO_BOOTABLE_FOUND
    jz .error
    .bootable_found:
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_l], cx
    mov word [lba_xfer_pkt + ltp_lba_addr_lower32_h], dx
    mov word [lba_xfer_pkt + ltp_num_sector], 1
    mov word [lba_xfer_pkt + ltp_mem_offset], VBR_ADDRESS
    mov word [lba_xfer_pkt + ltp_mem_segment], 0
    mov si, lba_xfer_pkt
    mov al, byte [BOOT_DISK_ID_VAR] ; (0x80|DiskID)
    call lba_send_transfer_packet
    test al, al
    mov si, FAIL_LBA_READ
    jz .error
    .lba_read_success:
    mov si, BOOTING_VBR_MSG
    call vga_print_cstr
    jmp third_stage ; noreturn
    .error:
    call vga_clear_scr
    call vga_print_cstr
    .halt: ; Maybe shutdown here instead?
    cli
    hlt
    jmp .halt

third_stage:
    ; OS/2 VBR Interface
    mov dl, byte [BOOT_DISK_ID_VAR]
    mov si, word [MBR_BOOT_PTE_VAR]
    mov ax, MBR_COPY_SEGMENT
    mov ds, ax
    mov si, bx ; bx is the offset of the bootable PTE, set on the last mbr_get_vbr_lba call
    jmp 0x0000:VBR_ADDRESS

; BX=Offset of the bootable PTE in MBR, CX=Lower 16 Bits, DX=Higher 16 bits, AL=0 indicates failure
mbr_get_vbr_lba:
    mov cx, 3 ; iterate 4 partition entries
    mov bx, MBR_PT_OFFSET
    .loop:
    mov al, byte [bx + mp_status]
    add bx, MBR_PTE_SIZE
    cmp al, 0x80
    loopne .loop
    jz .success
    .fail:
    xor ax, ax
    ret
    .success:
    mov ax, 1  ; Indicates success
    sub bx, MBR_PTE_SIZE
    mov cx, word [bx + mp_lba_first_l]
    mov dx, word [bx + mp_lba_first_h]
    ret
