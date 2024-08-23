#!/bin/bash

set -e
SRC_DIR=$PWD

source ../edksetup.sh
build -a X64 -t GCC5 -p MdeModulePkg/MdeModulePkg.dsc
cp ../Build/MdeModule/DEBUG_GCC5/X64/Pong.efi $SRC_DIR/BOOTX64.EFI

mkdir -p image/EFI/BOOT
cp ./BOOTX64.EFI image/EFI/BOOT/

qemu-system-x86_64 -net none -bios $SRC_DIR/OVMF_dir/OVMF.fd -drive file=fat:rw:image,media=disk,format=raw