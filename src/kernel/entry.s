[BITS 32]

extern kmain
extern load_kernel_gdt
extern load_kernel_idt
extern enable_all_interrupts
extern memory_virt_init

extern memory_phy_bitmap
extern memory_phy_bitmap_count
extern paging_pPagingStruct

global _entry:function

[SECTION .init]
_entry:
    mov eax, dword [esp+4]
    mov dword [memory_phy_bitmap], eax
    mov eax, dword [esp+8]
    mov dword [memory_phy_bitmap_count], eax
    mov eax, dword [esp+12]
    mov dword [paging_pPagingStruct], eax
    call load_kernel_gdt
    call load_kernel_idt
    call memory_virt_init
    ;TODO: memory_virt_reserve oldBitmap, paging structures, the kernel executable and the stack here
    call kmain
    .halt:
    hlt
    jmp .halt