all: root/efi/boot/bootx64.efi

root/efi/boot/bootx64.efi: build/bootx64.efi
	cp build/bootx64.efi root/efi/boot/bootx64.efi

build/bootx64.efi: build/main.o build/input.o
	lld \
		-flavor link \
		-subsystem:efi_application \
		-entry:efi_main \
		-out:build/bootx64.efi \
		build/main.o \
		build/input.o

define compile =
	echo $1
	clang \
		-c src/$1.c \
		-o build/$1.o \
		-ffreestanding \
		-MMD \
		-mno-red-zone \
		-std=c11 \
		-target x86_64-unknown-windows
endef

build/main.o: src/main.c
	$(call compile,main)

build/input.o: src/input.c
	$(call compile,input)

test: root/efi/boot/bootx64.efi
	qemu-system-x86_64 \
  -drive if=pflash,format=raw,file=OVMF.fd \
  -drive format=raw,file=fat:rw:root \
  -net none
