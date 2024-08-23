#!/bin/bash

set -e
SRC_DIR=$PWD

# Compile main.c in EDKII
source ../edksetup.sh
build -a X64 -t GCC5 -p MdeModulePkg/MdeModulePkg.dsc
cp ../Build/MdeModule/DEBUG_GCC5/X64/Pong.efi $SRC_DIR/BOOTX64.EFI

# Create the UEFI image
mkdir -p image/EFI/BOOT
cp ./BOOTX64.EFI image/EFI/BOOT/
