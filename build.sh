#!/bin/bash

set -e

SRC_DIR=$PWD
source ../edksetup.sh
build -a X64 -t GCC5 -p MdeModulePkg/MdeModulePkg.dsc

cp ../Build/MdeModule/DEBUG_GCC5/X64/Pong.efi ./BOOTX64.EFI
cp BOOTX64.EFI hda-contents/
qemu-system-x86_64 -pflash $SRC_DIR/OVMF_dir/OVMF.fd -hda fat:rw:hda-contents