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


