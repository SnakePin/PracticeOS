OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    SEG1 (rwx) : ORIGIN = 0x7C00, LENGTH = 1M
}

SECTIONS
{
    __executable_start = .;
    /* Stage 1 */
    .text :
    {
        *(.init .text)
    }
    .data : 
    {
        *(.data)
    }

    /* Stage 2 */
    .text2 :
    {
        *(.text2)
    }
    .data2 : 
    {
        *(.data2)
    }
    __vbr_size = . - __executable_start;
    /DISCARD/ : {*(*)}
}