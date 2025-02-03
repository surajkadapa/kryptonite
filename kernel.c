typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n){
	uint8_t *p = (uint8_t *) buf;
	while(n--)
		*p++ = c;
	return buf;
}

void kernel_main(void){
	memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

	for(;;);
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
