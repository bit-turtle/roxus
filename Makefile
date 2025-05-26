all: root/efi/boot/bootx64.efi

root/efi/boot/bootx64.efi: root/efi/boot build/bootx64.efi
	cp build/bootx64.efi root/efi/boot/bootx64.efi

build/bootx64.efi: build/main.o
	lld \
		-flavor link \
		-subsystem:efi_application \
		-entry:efi_main \
		build/main.o \
		-out:build/bootx64.efi

build/main.o: src/main.c
	clang \
		-c src/main.c \
		-o build/main.o \
		-ffreestanding \
		-MMD \
		-mno-red-zone \
		-std=c11 \
		-target x86_64-unknown-windows

test: root/efi/boot/bootx64.efi
	qemu-system-x86_64 \
  -drive if=pflash,format=raw,file=OVMF.fd \
  -drive format=raw,file=fat:rw:root \
  -net none
