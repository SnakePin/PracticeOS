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
    PROVIDE(__executable_start = .);
    .text :
    {
        *(.init .text)
    }
    .data : 
    {
        *(.data)
    }
    __mbr_bootstrap_size = . - __executable_start;

    /* Stabs debugging sections.  */
    .stab          0 (NOLOAD) : { *(.stab) }
    .stabstr       0 (NOLOAD) : { *(.stabstr) }
    .stab.excl     0 (NOLOAD) : { *(.stab.excl) }
    .stab.exclstr  0 (NOLOAD) : { *(.stab.exclstr) }
    .stab.index    0 (NOLOAD) : { *(.stab.index) }
    .stab.indexstr 0 (NOLOAD) : { *(.stab.indexstr) }
    .comment       0 (NOLOAD) : { *(.comment) }
    .gnu.build.attributes 0 (NOLOAD) : { *(.gnu.build.attributes .gnu.build.attributes.*) }

    /DISCARD/ : {*(*)}
    ASSERT(__mbr_bootstrap_size <= 446, "Error: MBR is larger than 446 bytes")
}