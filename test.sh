pushd .
SCRIPT_DIR=$(dirname "$0") # $0 is the script's path
cd "$SCRIPT_DIR"
tup && qemu-system-x86_64 -M q35 -m 1G -enable-kvm \
  -drive format=raw,file=fat:rw:root \
  -drive if=pflash,format=raw,file=qemu/OVMF.fd \
  -drive if=pflash,format=raw,file=qemu/OVMF_VARS.fd \
  -device qemu-xhci,id=xhci \
  -device usb-mouse \
  -nic user,model=virtio-net-pci
popd
