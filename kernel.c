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

void *memset(void *buf, char c, size_t n){
	uint8_t *p = (uint8_t *) buf;
	while(n--)
		*p++ = c;
	return buf;
}

void kernel_main(void){
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

	const char *s = "\n\nHello World!\n";
	for(int i=0; s[i] != '\0'; i++){
		putchar(s[i]);
	}

  printf("\nNEW Hello World %s \n", "YOLOOOO");
  printf("1 + 3 = %d, %x\n", 1+3, 0x1234abcd);

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
