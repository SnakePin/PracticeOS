Assembly files are compiled using NASM, C files are compiled using GCC.
Installing dependencies:
```sh
sudo apt-get install -y dotnet-runtime-6.0
dotnet tool install -g dotnet-script
cat << EOF >> ~/.profile
# Add .NET Core SDK tools
export PATH="\$PATH:$HOME/.dotnet/tools"
EOF
. ~/.profile
sudo rm -rf ~/.cache
```

To build:
```
make all
```