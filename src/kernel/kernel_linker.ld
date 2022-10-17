OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    RAM (rwx) : ORIGIN = 0, LENGTH = 4096M
}

SECTIONS
{
    PROVIDE(__executable_start = .);

    /* Code */
    .text :
    {
        *(.init)
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(SORT(.text.sorted.*))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }

    /* Read-only data */
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }

    /* Initialized data */
    .data : 
    {
        *(.sdata .sdata.*)
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.* .gnu.linkonce.d.*)
        PROVIDE(__data_end = .);
    }

    /* Uninitialized data */
    .bss :
    {
        *(.bss*)
        *(.sbss*)
    }

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
}