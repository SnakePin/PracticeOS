OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    SEG1 (rwx) : ORIGIN = 0, LENGTH = 1M
}

SECTIONS
{
    /* Origin at 0, we'll use segment registers instead */
    __executable_start = .;
    .text :
    {
        *(.init .text)
    }
    .data : 
    {
        *(.data)
    }
    /DISCARD/ : {*(*)}
    ASSERT((. - __executable_start) <= 446, "Error: MBR is larger than 446 bytes")
}