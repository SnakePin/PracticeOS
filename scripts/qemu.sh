#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
qemu-system-i386 -drive format=raw,media=disk,file="$SCRIPT_DIR/../out/disk.img" "$@"