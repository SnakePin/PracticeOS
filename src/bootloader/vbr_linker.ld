OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    SEG1 (rwx) : ORIGIN = 0x7c00, LENGTH = 1M
}

SECTIONS
{
    . = 0x7c00;
    PROVIDE(__executable_start = .);
    /* Stage 1, must be able to access utils */
    .text ALIGN(1) : { *(.init) *(.text) }
    .data ALIGN(1) : { *(.data) }
    ASSERT((. - __executable_start) <= 512, "Error: VBR loader is larger than 512 bytes")
    
    /* Stage 2 */
    .text_stage2 ALIGN(1) : { *(.text_stage2) }
    .data_stage2 ALIGN(1) : { *(.data_stage2) }
    __vbr_size = . - __executable_start;
    
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
    ASSERT(__vbr_size <= 65536, "Error: VBR is larger than 64KiB")
}