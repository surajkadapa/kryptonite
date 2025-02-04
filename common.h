#pragma once

typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;
typedef uint32_t paddr_t; //to represent physical memory address
typedef uint32_t vaddr_t; //to represent virtual memory address

#define true 1
#define false 0
#define NULL ((void *) 0)
#define align_up(value, align) __builtin_align_up(value, align) //rounds up to the nearest multiple of align, align must be a power of 2
#define is_aligned(value, align) __builtin_is_aligned(value, align) //checks if value is a multiple of align, align must be a power of 2
#define offsetof(type, member) __builtin_offsetof(type, memeber) //returns the offset of a member within a structure
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
char *strcpy_s(char *dst, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
void printf(const char *fmt, ...);
