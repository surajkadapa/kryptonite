QEMU=qemu-system-riscv32

$QEMU -machine virt -bios default \
  -kernel kernel.elf \
  -serial stdio \
  -monitor telnet:127.0.0.1:1234,server,nowait \
  -qmp unix:./qmp-sock,server,nowait \
  -d guest_errors,cpu_reset,in_asm,exec \
  -D qemu.log \
  --no-reboot
