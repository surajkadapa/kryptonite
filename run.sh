#!/bin/bash

#qemu path
QEMU=qemu-system-riscv32

#starting qemu
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot
