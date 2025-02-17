#include "kernel.h"
#include "common.h"
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid){
	register long a0 __asm__("a0") = arg0;
	register long a1 __asm__("a1") = arg1;
	register long a2 __asm__("a2") = arg2;
	register long a3 __asm__("a3") = arg3;
	register long a4 __asm__("a4") = arg4;
	register long a5 __asm__("a5") = arg5;
	register long a6 __asm__("a6") = fid;
	register long a7 __asm__("a7") = eid;

	__asm__ __volatile__("ecall"
		: "=r" (a0), "=r"(a1)
		: "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
		: "memory");
	return (struct sbiret){.error = a0, .value = a1};

}

void putchar(char ch){
	sbi_call(ch, 0, 0, 0, 0, 0, 0, 1);
}
extern char __free_ram[], __free_ram_end[];

paddr_t alloc_pages(uint32_t n){
	static paddr_t next_paddr = (paddr_t) __free_ram;  //value of next_paddr is retained across functions, behaving like a global variable
	paddr_t paddr = next_paddr;
	next_paddr += n * PAGE_SIZE;

	if(next_paddr > (paddr_t) __free_ram_end)
		PANIC("OUT OF MEMORY");
	memset((void *) paddr, 0, n*PAGE_SIZE);
	return paddr;
}
void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags){
  if(!is_aligned(vaddr, PAGE_SIZE))
    PANIC("[SYSTEM] unaligned vaddr %x", vaddr);
  if(!is_aligned(paddr, PAGE_SIZE))
    PANIC("[SYSTEM] unaligned paddr %x", paddr);

  uint32_t vpn1 = (vaddr >> 22) & 0x3ff;
  if ((table1[vpn1] & PAGE_V) == 0){
    //creating the 2nd lvl page table that doesnt exist
    uint32_t pt_addr = alloc_pages(1);
    table1[vpn1] = ((pt_addr / PAGE_SIZE) << 10) | PAGE_V;
  } 

  uint32_t vpn0 = (vaddr >> 12) & 0x3ff;
  uint32_t *table0 = (uint32_t *) ((table1[vpn1] >> 10) * PAGE_SIZE);
  table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}
__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void){
  __asm__ __volatile__(
    //retrive the kernel stack of the current running process from the stack
    "csrrw sp, sscratch, sp\n"

    "addi sp, sp, - 4 * 31\n"
    "sw ra, 4 * 0(sp)\n"
    "sw gp, 4 * 1(sp)\n"
    "sw tp, 4 * 2(sp)\n"
    "sw t0, 4 * 3(sp)\n"
    "sw t1, 4 * 4(sp)\n"
    "sw t2, 4 * 5(sp)\n"
    "sw t3, 4 * 6(sp)\n"
    "sw t4, 4 * 7(sp)\n"
    "sw t5, 4 * 8(sp)\n"
    "sw t6, 4 * 9(sp)\n"
    "sw a0, 4 * 10(sp)\n"
    "sw a1, 4 * 11(sp)\n"
    "sw a2, 4 * 12(sp)\n"
    "sw a3, 4 * 13(sp)\n"
    "sw a4, 4 * 14(sp)\n"
    "sw a5, 4 * 15(sp)\n"
    "sw a6, 4 * 16(sp)\n"
    "sw a7, 4 * 17(sp)\n"
    "sw s0, 4 * 18(sp)\n"
    "sw s1, 4 * 19(sp)\n"
    "sw s2, 4 * 20(sp)\n"
    "sw s3, 4 * 21(sp)\n"
    "sw s4, 4 * 22(sp)\n"
    "sw s5, 4 * 23(sp)\n"
    "sw s6, 4 * 24(sp)\n"
    "sw s7, 4 * 25(sp)\n"
    "sw s8, 4 * 26(sp)\n"
    "sw s9, 4 * 27(sp)\n"
    "sw s10, 4 * 28(sp)\n"
    "sw s11, 4 * 29(sp)\n"

    "csrr a0, sscratch\n"
    "sw a0, 4 * 30(sp)\n"

    //reset the kernel stack
    "addi a0, sp, 4 * 31\n"
    "csrw sscratch, a0\n"

    "mv a0, sp\n"
    "call handle_trap\n"

    "lw ra, 4 * 0(sp)\n"
    "lw gp, 4 * 1(sp)\n"
    "lw tp, 4 * 2(sp)\n"
    "lw t0, 4 * 3(sp)\n"
    "lw t1, 4 * 4(sp)\n"
    "lw t2, 4 * 5(sp)\n"
    "lw t3, 4 * 6(sp)\n"
    "lw t4, 4 * 7(sp)\n"
    "lw t5, 4 * 8(sp)\n"
    "lw t6, 4 * 9(sp)\n"
    "lw a0, 4 * 10(sp)\n"
    "lw a1, 4 * 11(sp)\n"
    "lw a2, 4 * 12(sp)\n"
    "lw a3, 4 * 13(sp)\n"
    "lw a4, 4 * 14(sp)\n"
    "lw a5, 4 * 15(sp)\n"
    "lw a6, 4 * 16(sp)\n"
    "lw a7, 4 * 17(sp)\n"
    "lw s0, 4 * 18(sp)\n"
    "lw s1, 4 * 19(sp)\n"
    "lw s2, 4 * 20(sp)\n"
    "lw s3, 4 * 21(sp)\n"
    "lw s4, 4 * 22(sp)\n"
    "lw s5, 4 * 23(sp)\n"
    "lw s6, 4 * 24(sp)\n"
    "lw s7, 4 * 25(sp)\n"
    "lw s8, 4 * 26(sp)\n"
    "lw s9, 4 * 27(sp)\n"
    "lw s10, 4 * 28(sp)\n"
    "lw s11, 4 * 29(sp)\n"
    "lw sp, 4 * 30(sp)\n"
    "sret\n"
  );
}



void handle_trap(struct trap_frame *f){
  uint32_t scause = READ_CSR(scause);
  uint32_t stval = READ_CSR(stval);
  uint32_t user_pc = READ_CSR(sepc);

  PANIC("Unexpected Trap scause = %x, stval=%x, spec=%x\n", scause, stval, user_pc);
}

__attribute__((naked)) void context_switch(uint32_t *prev_sp, uint32_t *next_sp){
	__asm__ __volatile__(
		//save the callee registers onto the current process's stack
		"addi sp, sp, -13 * 4\n" //allocating space for 13 4-byte registers
		"sw ra, 4 * 0(sp)\n"
		"sw s0, 4 * 1(sp)\n"
		"sw s1, 4 * 2(sp)\n"
		"sw s2, 4 * 3(sp)\n"
		"sw s3, 4 * 4(sp)\n"
		"sw s4, 4 * 5(sp)\n"
		"sw s5, 4 * 6(sp)\n"
		"sw s6, 4 * 7(sp)\n"
		"sw s7, 4 * 8(sp)\n"
		"sw s8, 4 * 9(sp)\n"
		"sw s9, 4 * 10(sp)\n"
		"sw s10, 4 * 11(sp)\n"
		"sw s11, 4 * 12(sp)\n"

		//switch the stack pointer
		"sw sp, (a0)\n" //*prev_sp = sp
		"lw sp, (a1)\n" //switching the stack pointer here

		//restore callee-saved registers
		"lw ra, 4 * 0(sp)\n"
		"lw s0, 4 * 1(sp)\n"
		"lw s1, 4 * 2(sp)\n"
		"lw s2, 4 * 3(sp)\n"
		"lw s3, 4 * 4(sp)\n"
		"lw s4, 4 * 5(sp)\n"
		"lw s5, 4 * 6(sp)\n"
		"lw s6, 4 * 7(sp)\n"
		"lw s7, 4 * 8(sp)\n"
		"lw s8, 4 * 9(sp)\n"
		"lw s9, 4 * 10(sp)\n"
		"lw s10, 4 * 11(sp)\n"
		"lw s11, 4 * 12(sp)\n"
		"addi sp, sp, 13 * 4\n"
		"ret\n"
	);
}

struct process procs[PROCS_MAX];
extern char __kernel_base[];


struct process *create_process(uint32_t pc){
  struct process *proc=NULL;
  int i;
  for(i = 0; i < PROCS_MAX; i++){ //finding an unused process control block
    if(procs[i].state == PROC_UNUSED){
      proc = &procs[i];
      break;
    }
  }

  if(!proc)
    PANIC("[ERROR]NO FREE PROCESS CONTROL BLOCKS!");

  uint32_t *sp = (uint32_t *) &proc->stack[sizeof(proc->stack)];
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = (uint32_t) pc;

  //map kernel pages
  uint32_t *page_table = (uint32_t *) alloc_pages(1);
  for(paddr_t paddr = (paddr_t) __kernel_base; paddr < (paddr_t) __free_ram_end; paddr += PAGE_SIZE)
    map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);

  //init fields
  proc->pid = i + 1;
  proc->state = PROC_RUNNABLE;
  proc->sp = (uint32_t) sp;
  proc->page_table = page_table;
  return proc;
}

void delay(void){
  for(int i = 0; i < 30000000; i++)
    __asm__ __volatile__("nop");
}

struct process *current_proc;
struct process *idle_proc; //used when there are no other process to run

//process giving up control voluntarily
void yield(void){
  //search for a runnable process
  struct process *next = idle_proc;
  for(int i = 0; i < PROCS_MAX; i++){
    struct process *proc = &procs[(current_proc->pid + i) % PROCS_MAX]; //find the next runnable process using round robin
    if(proc->state == PROC_RUNNABLE && proc->pid > 0){
      next = proc;
      break;
    }
  }

  if (next == current_proc)
    return;

  __asm__ __volatile__(
    "csrw sscratch, %[sscratch]\n"
    :
    : [sscratch] "r" ((uint32_t) &next->stack[sizeof(next->stack)])
  );

  
  //context switching
  struct process *prev = current_proc;
  current_proc = next;
  //switch the process's page table
  __asm__ __volatile__(
      "sfence.vma\n"
      "csrw satp, %[satp]\n"
      "sfence.vma\n"
      "csrw sscratch, %[sscratch]\n"
      :
      : [satp] "r" (SATP_SV32 | ((uint32_t) next->page_table / PAGE_SIZE)),
        [sscratch] "r" ((uint32_t) &next->stack[sizeof(next->stack)])
  );
  context_switch(&prev->sp, &next->sp);
}

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void){
  printf("[SYSTEM PROC A] STARTING process A\n");
  while(1){
    putchar('A');
    yield();
    delay();
  }
}

void proc_b_entry(void){
  printf("[SYSTEM PROC B] STARTING process b\n");
  while(1){
    putchar('B');
    yield();
    delay();
  }
}



void kernel_main(void){
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
	WRITE_CSR(stvec, (uint32_t) kernel_entry);
	//__asm__ __volatile__("unimp"); //instruction that triggers an illegal instruction exception

	paddr_t paddr0 = alloc_pages(2);
	paddr_t paddr1 = alloc_pages(1);
	printf("[test]alloc_pages: paddr0=%x\n", paddr0);
	printf("[test]alloc_pages: paddr1=%x\n", paddr1);



	//const char *s = "\n\nHello World!\n";
	//for(int i=0; s[i] != '\0'; i++){
	//	putchar(s[i]);
	//}
	//
	//PANIC("BOOTED");

  printf("\nBOOTED\n");
  // printf("1 + 3 = %d, %x\n", 1+3, 0x1234abcd);
  //
  idle_proc = create_process((uint32_t) NULL);
  idle_proc->pid = -1;
  current_proc = idle_proc;

  proc_a = create_process((uint32_t) proc_a_entry);
  proc_b = create_process((uint32_t) proc_b_entry);
  
  yield();
  PANIC("[SYSTEM] EXECUTING IDLE\n");

	for(;;){
		__asm__ __volatile__("wfi"); //wfi - wait for interrupt, conserve power by putting the CPU core into a low power state
	}
}

__attribute__((section(".text.boot"))) //need this so the function is placed at the memory address where openSBI will look
__attribute__((naked)) //to tell the compiler to not generate any code before or after the function, ensuring the inline assembly code is the same as the function body
void boot(void){
	__asm__ __volatile__(
		"mv sp, %[stack_top]\n" //stack pointer set up
		"j kernel_main\n"
		:
		: [stack_top] "r" (__stack_top) //passing the stack top address(stack_top is set in kernel.ld)
	);
}
