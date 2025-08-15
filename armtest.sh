pushd .
SCRIPT_DIR=$(dirname "$0") # $0 is the script's path
cd "$SCRIPT_DIR"
tup
qemu-system-aarch64 \
	-machine type=virt,iommu=smmuv3 \
	-accel tcg \
	-cpu cortex-a57 \
	-m 2048 \
	-rtc base=utc \
	-drive if=pflash,format=raw,file="QEMU_EFI.fd",readonly=on \
	-drive if=pflash,format=raw,file="QEMU_VARS.fd" \
	-drive format=raw,file=fat:rw:root \
	-boot menu=on \
	-display gtk \
	-device virtio-gpu \
	-device qemu-xhci \
	-device usb-kbd \
	-device usb-tablet
popd
