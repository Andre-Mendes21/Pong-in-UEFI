#!/bin/bash

SRC_DIR=$PWD

# Boot qemu
qemu-system-x86_64 -net none -bios $SRC_DIR/OVMF.fd -drive file=fat:rw:image,media=disk,format=raw
