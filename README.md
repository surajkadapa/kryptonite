# Kryptonite

**Kryptonite** is a minimalist 32-bit RISC-V kernel built from scratch for educational and systems exploration purposes. It runs on QEMU using OpenSBI and demonstrates core OS concepts including memory management, trap handling, and cooperative multitasking.

## Features

- Bare-metal kernel written in C (freestanding, no libc)
- Boots on QEMU `riscv32-virt` using OpenSBI
- Custom linker script defining memory layout
- SBI console output (`putchar`, `printf`)
- Manual zeroing of `.bss` section during boot
- Page-aligned bump allocator (`alloc_pages`)
- SV32 two-level paging setup (virtual memory)
- Per-process page tables with identity mappings
- Process structure with PID, stack, and address space
- Cooperative multitasking with explicit `yield()`
- Full context switching (`context_switch`) with callee-saved registers
- Trap handling via `stvec`, `sscratch`, and `kernel_entry`
- Debug support with QEMU logs and monitor scripts

---

## File Structure

`kernel.c` - Main kernel logic, scheduler, trap handler, entry points\
`kernel.h` - Core definitions (CSR macros, structs, flags)\
`common.c` - Custom libc functions (printf, memset, memcpy)\
`common.h` - Basic typedefs and helper macros\
`kernel.ld` - Linker script for memory layout and stack setup\
`run.sh` - Compiles and runs the kernel via QEMU\
`debug.sh` - QEMU debug run with monitor and logs\
`monitor.cmd` - Logs PC during execution (QEMU monitor script)\

---


## Build and Run

### Requirements
- `clang` or `riscv32-unknown-elf-gcc`
- `qemu-system-riscv32`

### Run (default)
```bash
./run.sh
```

### Run with Debugging
```bash
./debug.sh
```


## Architecture Overview

Boot Flow:

- QEMU runs OpenSBI → jumps to `boot` at 0x80200000

- `boot` sets stack and jumps to `kernel_main`

- `.bss` cleared, traps set up, processes created


Multitasking:

- `create_process` allocates stack + page table

- `yield()` selects next runnable process (round-robin)

- `context_switch` saves/restores registers and stacks

Trap Handling:

- `stvec` points to `kernel_entry` (assembly)

- All registers saved in `trap_frame`, passed to `handle_trap`

- Traps currently unhandled → PANIC + debug output

## Notes

Each process runs in supervisor mode with its own identity-mapped page table. 

All memory mapped with RWX permissions (no isolation yet). 

No user-mode, syscall interface, or interrupts implemented.

---



