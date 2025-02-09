#include "common.h"
#pragma once

#define PANIC(fmt, ...) \
	do{ \
		printf("KERNEL_PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
		while(1){} \
	} while (0) //this method to make sure the statements here execute atleast once

struct sbiret {
	long error;
	long value;
};


struct trap_frame {
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t sp;
} __attribute__((packed));

#define READ_CSR(reg) \
({ \
	unsigned long __temp; \
	__asm__ __volatile__("csrr %0, " #reg : "=r"(__temp)); \
	__temp; \
})

#define WRITE_CSR(reg, value) \
	do{ \
	uint32_t __temp = (value); \
	__asm__ __volatile__("csrw " #reg ", %0" :: "r"(__temp)); \
	}while(0)

//macros and struct needed for processes
#define PROCS_MAX 8 //maximum number of proceses
#define PROC_UNUSED 0
#define PROC_RUNNABLE 1 

//proccess control block(pcb)
struct process {
	int pid; //process id
	int state; //process state: PROC_UNUSED or PROC_RUNNABLE
	vaddr_t sp; //stack pointer
	uint8_t stack[8192]; //kernel stack
	//contains saved CPU regs, return addresses and local vars, can be used for context switching
};



