#include "common.h"

void putchar(char ch);

void printf(const char *fmt, ...){
  va_list vargs;
  va_start(vargs, fmt);

  while(*fmt){
    if (*fmt == '%'){
      fmt++; //for skipping the "%"
      switch(*fmt){
        case '\0': //% is at the end of the format string
          putchar('%');
          goto end;
        case 's':{ //string
          const char *s = va_arg(vargs, const char *);
          while (*s){
            putchar(*s);
            s++;
          }
          break;
        }
        case 'd':{ //integer in decimal
          int value = va_arg(vargs, int);
          if(value < 0){
            putchar('-');
            value = -value;
          }
          int divisor = 1;
          while(value/divisor > 9)
            divisor *= 10;
          while(divisor > 0){
            putchar('0' + value/divisor);
            value %= divisor;
            divisor /= 10;
          }
          break;
        }
        case 'x':{ //for a hexadecimal integer
          int value = va_arg(vargs, int);
          for(int i=7;i >= 0; i--){
            int nibble = (value >> (i*4)) & 0xf;
            putchar("0123456789abcdef"[nibble]);
          }
        }
      }
    }else{
      putchar(*fmt);
    }
    fmt++;
  }
  end:
    va_end(vargs);
}

void *memcpy(void *dst, const void *src, size_t n){
  uint8_t *d = (uint8_t *) dst;
  const uint8_t *s = (const uint8_t *) src;
  while (n--)
    *d++ = *s++;
  return dst;
}

void *memset(void *buf, char c, size_t n){
  uint8_t *p = (uint8_t *) buf;
  while (n--)
    *p++ = c;
  return buf;
}

char *strcpy(char *dst, const char *src){
  char *d = dst;
  while (*src)
    *d++ = *src++;
  *d = '\0';
  return dst;
}

char *strcpy_s(char *dst, const char *src, size_t n){ //ensure that the string does not continue copying to dst if dest is smaller than src
  char *d = dst;
  while(n > 1 && *src){
    *dst++ = *src++;
    n--;
  }
  *d = '\0'; //ensuring null termination
  return dst;
}

int strcmp(const char *s1, const char *s2){
  while(*s1 && *s2){
    if(*s1 != *s2)
      break;
    s1++;
    s2++;
  }

  return *(unsigned char *)s1 - *(unsigned char *)s2;
}
