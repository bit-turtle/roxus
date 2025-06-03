tup
qemu-system-x86_64 \
  -drive if=pflash,format=raw,file=OVMF.fd \
  -drive format=raw,file=fat:rw:root \
  -net none
