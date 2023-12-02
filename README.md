## Info
Assembly files are compiled using NASM, C files are compiled using GCC.\
Some scripts used in the build process require Python 3

## Running in emulator
1. Commands for bochs:
```sh
sudo apt install -y bochs-x bochsbios vgabios
./scripts/bochs.sh
```
2. Commands for QEMU:
```sh
sudo apt install -y qemu-system-i386
./scripts/qemu.sh
```

## Building
**Note**: NASM must be newer than 2.16 or you have to compile NASM from source with [this patch](https://github.com/iglosiggio/nasm/commit/eb5d3b7c6a79e485c98e8c6aa8dbde8d1bce6c6d) to have proper debug symbols for asm files.

Command:
```sh
make all
```