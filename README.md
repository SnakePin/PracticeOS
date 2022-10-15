Assembly files are compiled using NASM, C files are compiled using GCC.
You need to install dotnet-script, you can use the following command to do so:
```sh
dotnet tool install -g dotnet-script
```

To build:
```
./compile_all.ps1
dotnet script mkdisk.cs
```