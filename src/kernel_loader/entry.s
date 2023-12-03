[BITS 32]

; BEGIN EXTERN/GLOBAL DEFINITIONS
extern ldrmain
extern load_kernel_gdt
global _entry:function
; END EXTERN/GLOBAL DEFINITIONS

[SECTION .data]
BOOT_PAR_LBA_VAR dd 0
BOOT_DISK_ID_VAR dd 0

[SECTION .init]
_entry:
    ; A custom interface with the VBR, BOOT_PAR_LBA @ eax, BOOT_DISK_ID_VAR @ ebx
    mov dword [BOOT_PAR_LBA_VAR], eax
    mov dword [BOOT_DISK_ID_VAR], ebx
    mov esp, 0x80000 ; Just below EBDA
    call load_kernel_gdt
    push dword [BOOT_PAR_LBA_VAR]
    push dword [BOOT_DISK_ID_VAR]
    call ldrmain
    add esp, 8
    .halt:
    hlt
    jmp .halt