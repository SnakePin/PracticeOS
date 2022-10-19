## Info
Assembly files are compiled using NASM, C files are compiled using GCC.\
Some scripts used in the build process require .NET runtime.

## Prerequisites
Installing dependencies:
```sh
sudo apt install -y dotnet-runtime-6.0
dotnet tool install -g dotnet-script
cat << EOF >> ~/.profile
# Add .NET Core SDK tools
export PATH="\$PATH:$HOME/.dotnet/tools"
EOF
. ~/.profile
sudo rm -rf ~/.cache
```

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
**Note**: You must compile NASM from source with [this patch](https://github.com/iglosiggio/nasm/commit/eb5d3b7c6a79e485c98e8c6aa8dbde8d1bce6c6d) to have proper debug symbols for asm files.

Command:
```sh
make all
```