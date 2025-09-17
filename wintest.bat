@echo off
REM Test building on Windows
REM Tupfile wasn't working on windows for some reason
mkdir build
cd build
clang -ffreestanding -mno-red-zone -std=c11 -target x86_64-unknown-windows -c ..\src\*.c
cd ..
lld -flavor link -out:build/bootx64.efi build/*.o -subsystem:efi_application -entry:efi_main
mkdir root\efi\boot
copy build\bootx64.efi root\efi\boot\
xcopy /Y roxus\ root\roxus\
qemu-system-x86_64 -drive if=pflash,format=raw,file=qemu/OVMF.fd -drive if=pflash,format=raw,file=qemu/OVMF_VARS.fd -drive format=raw,file=fat:rw:root -usb -device usb-tablet -nic user,model=virtio-net-pci
