#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SRC_DIR=$(readlink -f "$SCRIPT_DIR/../../")

# For real mode debugging: -ix gdb_init_rm.txt
gdb -ex "set confirm off" \
	-ex "set substitute-path /work \"$SRC_DIR\"" \
	-ex "add-symbol-file \"$SRC_DIR/out/bootloader/mbr.elf\" 0" \
	-ex "add-symbol-file \"$SRC_DIR/out/bootloader/vbr.elf\" 0x7C00" \
	-ex "add-symbol-file \"$SRC_DIR/out/kernel_loader/kernel_loader.elf\" 0x1000" \
	-ex "add-symbol-file \"$SRC_DIR/out/kernel/kernel.elf\" 0xC0000000" \
	"$@"