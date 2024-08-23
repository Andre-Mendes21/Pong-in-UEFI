# Pong in UEFI

A simple x86-64 [UEFI](https://wiki.osdev.org/UEFI) application of the game [Pong](https://en.wikipedia.org/wiki/Pong) using the [EDK II](https://github.com/tianocore/edk2) reference implementation of UEFI.

![image](https://github.com/user-attachments/assets/1280623a-040c-4a52-9c4f-7e461e466543)

## Pre-requisites

- [QEMU](https://qemu.org)
- Build of OVMF.fd (follow this [guide](https://github.com/tianocore/tianocore.github.io/wiki/How-to-run-OVMF))

## Building

1. Follow the steps in this [OSDev Wiki page](https://wiki.osdev.org/EDK2#Building_EDK2's_base) on how to build EDK II.

2. Inside the `root` folder of EDK II clone this repository.

3. Place the `OVMF.fd` file in the root folder of this project.

4. Run `build.sh`.

There should now be a folder called `image` containing the following structure `image/EFI/BOOT/` with the `BOOTX64.EFI` file inside.

## Running

- Run `run.sh`.

## Playing the Game

- `w` / `s` moves the left bat up and down, respectively.

- `arrow_up` / `arrow_down` moves the right bat up and down, respectively.
