Assembly files are compiled using NASM, C files are compiled using GCC.
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

Running in Bochs:
```sh
sudo apt install -y bochs-x bochsbios vgabios
./scripts/bochs.sh
```

Running in QEMU:
```sh
sudo apt install -y qemu-system-i386
./scripts/qemu.sh
```

To build:
```
make all
```