#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SRC_DIR=$(readlink -f "$SCRIPT_DIR/../../")

gdbgui -g "./gdb.sh" --project "$SRC_DIR" --remap-sources="{\"/work\": \"$SRC_DIR\"}"
