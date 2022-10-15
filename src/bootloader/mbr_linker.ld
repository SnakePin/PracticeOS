OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    /* Origin at 0, we'll use segment registers instead */
    SEG1 (rwx) : ORIGIN = 0x0000, LENGTH = 1M
}

SECTIONS
{
    .text :
    {
        *(.init .text)
    }
    .data : 
    {
        *(.data)
    }
    /DISCARD/ : {*(*)}
}