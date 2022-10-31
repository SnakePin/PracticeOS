[BITS 16]

%include "defs.s"

; BEGIN EXTERN DECLARATIONS
extern disable_all_interrupts
extern BOOT_DISK_ID_VAR     ; byte
extern BOOT_PAR_LBA_L_VAR   ; word
extern BOOT_PAR_LBA_H_VAR   ; word
; END EXTERN DECLARATIONS

[SECTION .data]
gdt_descriptor:
    gdt_size dw GDT_SIZE-1
    gdt_offset_l dw global_descriptor_table
    gdt_offset_h dw 0x0000
global_descriptor_table:
    istruc gdt_entry_t
    iend
    istruc gdt_entry_t ; Ring 0 Code
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_1010
        at gdte_flags_and_limit17_20, db 0b1100_1111
        at gdte_base_25_32, db 0x00
    iend
    istruc gdt_entry_t ; Ring 0 Data
        at gdte_limit, dw 0xFFFF
        at gdte_base, db 0,0,0
        at gdte_access, db 0b1001_0010
        at gdte_flags_and_limit17_20, db 0b1100_1111
        at gdte_base_25_32, db 0x00
    iend
GDT_SIZE EQU ($ - global_descriptor_table)

[SECTION .text]
global vbr_trampoline16:function
vbr_trampoline16:
    call disable_all_interrupts
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:vbr_trampoline32

[BITS 32]
vbr_trampoline32:
    ; We're in 32bit protected mode here
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    ; A custom interface, BOOT_PAR_LBA @ eax, BOOT_DISK_ID @ ebx
    xor ebx, ebx
    mov bl, byte [BOOT_DISK_ID_VAR]
    mov ax, word [BOOT_PAR_LBA_H_VAR]
    shl eax, 16
    mov ax, word [BOOT_PAR_LBA_L_VAR]
    jmp KERNEL_LDR_LOAD_FLAT_ADDR
