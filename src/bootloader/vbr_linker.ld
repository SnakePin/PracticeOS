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

    /* DWARF debug sections.
    Symbols in the DWARF debugging sections are relative to the beginning
    of the section so we begin them at 0.  */
    /* DWARF 1.  */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions.  */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2.  */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2.  */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions.  */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3.  */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF 5.  */
    .debug_addr     0 : { *(.debug_addr) }
    .debug_line_str 0 : { *(.debug_line_str) }
    .debug_loclists 0 : { *(.debug_loclists) }
    .debug_macro    0 : { *(.debug_macro) }
    .debug_names    0 : { *(.debug_names) }
    .debug_rnglists 0 : { *(.debug_rnglists) }
    .debug_str_offsets 0 : { *(.debug_str_offsets) }
    .debug_sup      0 : { *(.debug_sup) }
    .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }

    /DISCARD/ : {*(*)}
    ASSERT(__vbr_size <= 65536, "Error: VBR is larger than 64KiB")
}