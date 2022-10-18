OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_entry)

MEMORY
{
    /* Our assembly files are not PIC, therefore our executable is not PIE */
    RAM (rwx) : ORIGIN = 0x17BF0, LENGTH = 4096M
}

SECTIONS
{
    . = 0x17BF0;
    /* Code */
    .text ALIGN(1) :
    {
        *(.init)
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(SORT(.text.sorted.*))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }
    SDAHCXZG = .;

    /* Read-only data */
    .rodata ALIGN(1) : { *(.rodata .rodata.* .gnu.linkonce.r.*) }

    /* Initialized data */
    .data ALIGN(1) : 
    {
        *(.sdata .sdata.*)
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.* .gnu.linkonce.d.*)
    }

    /* Uninitialized data */
    .bss ALIGN(1) :
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