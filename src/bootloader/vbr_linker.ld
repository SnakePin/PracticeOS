OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    SEG1 (rwx) : ORIGIN = 0x7c00, LENGTH = 1M
}

SECTIONS
{
    __executable_start = 0x7c00; /* . is at 0 before any sections are defined */
    /* Stage 1, must be able to access utils */
    .text :
    {
        *(.init_text .text)
    }
    .data :
    {
        *(.init_data .data)
    }
    ASSERT((. - __executable_start) <= 512, "Error: VBR loader is larger than 512 bytes")
    
    /* Stage 2 */
    .text_stage2 :
    {
        *(.text_stage2)
    }
    .data_stage2 :
    {
        *(.data_stage2)
    }

    __vbr_size = . - __executable_start;
    /DISCARD/ : {*(*)}
    ASSERT((__vbr_size) <= 65536, "Error: VBR is larger than 64KiB")
}