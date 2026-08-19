#!/bin/bash
set -e

make

# Create and format virtual FAT disk using mtools (no sudo needed)
dd if=/dev/zero of=disk.img bs=1M count=64
mformat -i disk.img -F ::

# Copy UEFI application
mmd -i disk.img ::/EFI
mmd -i disk.img ::/EFI/BOOT
mcopy -i disk.img REBEL.EFI ::/EFI/BOOT/BOOTX64.EFI

# Copy template VARS file to your working directory if it doesn't exist
if [ ! -f my_vars.fd ]; then
    cp /usr/share/OVMF/OVMF_VARS_4M.fd my_vars.fd
fi

qemu-system-x86_64 \
    -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
    -drive if=pflash,format=raw,file=my_vars.fd \
    -drive file=disk.img,format=raw \
    -net none
