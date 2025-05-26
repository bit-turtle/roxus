clang -c src/main.c -o build/main.o -ffreestanding -MMD -mno-red-zone -std=c11 -target x86_64-unknown-windows
lld -flavor link -subsystem:efi_application -entry:efi_main build/main.o -out:build/bootx64.efi
cp build/bootx64.efi root/efi/boot
