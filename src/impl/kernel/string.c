/**
 * String Operations Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "../../intf/string.h"
#include "../../intf/stdlib.h"
#include <stdarg.h>

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    uint8_t* p = (uint8_t*)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }
    return s;
}

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) {
        len++;
    }
    return len;
}

char* strcpy(char* dest, const char* src) {
    size_t i = 0;
    while ((dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* ptr = dest + strlen(dest);
    while (*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    size_t dest_len = strlen(dest);
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strchr(const char* s, int c) {
    while (*s != (char)c) {
        if (!*s++) {
            return 0;
        }
    }
    return (char*)s;
}

char* strrchr(const char* s, int c) {
    const char* found = NULL;
    while (*s) {
        if (*s == (char)c) {
            found = s;
        }
        s++;
    }
    if (c == '\0') {
        return (char*)s;
    }
    return (char*)found;
}

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new = (char*)malloc(len);
    if (new) {
        memcpy(new, s, len);
    }
    return new;
}

/* Simple printf-style formatting */
static void print_num(char** str, uint64_t num, int base, int width, int pad_char) {
    char tmp[65];
    char* digits = "0123456789ABCDEF";
    int i = 0;

    /* Convert number to string */
    do {
        tmp[i++] = digits[num % base];
        num /= base;
    } while (num);

    /* Pad if necessary */
    while (i < width) {
        tmp[i++] = pad_char;
    }

    /* Reverse the string */
    while (i > 0) {
        *(*str)++ = tmp[--i];
    }
}

int vsprintf(char* str, const char* format, va_list args) {
    char* str_start = str;
    char c;
    int width;

    while ((c = *format++) != 0) {
        if (c != '%') {
            *str++ = c;
            continue;
        }

        /* Get width specifier */
        width = 0;
        if (*format >= '0' && *format <= '9') {
            width = *format++ - '0';
        }

        switch (*format++) {
            case 'd':
            case 'i':
                print_num(&str, va_arg(args, int), 10, width, ' ');
                break;
            case 'x':
            case 'X':
                print_num(&str, va_arg(args, unsigned int), 16, width, '0');
                break;
            case 'u':
                print_num(&str, va_arg(args, unsigned int), 10, width, ' ');
                break;
            case 's': {
                const char* s = va_arg(args, const char*);
                while (*s) {
                    *str++ = *s++;
                }
                break;
            }
            case 'c':
                *str++ = (char)va_arg(args, int);
                break;
            case '%':
                *str++ = '%';
                break;
            default:
                *str++ = '?';
                break;
        }
    }

    *str = '\0';
    return str - str_start;
}

int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsprintf(str, format, args);
    va_end(args);
    return ret;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    if (size == 0) return 0;
    
    va_list args;
    va_start(args, format);
    
    char* temp = (char*)malloc(size);
    if (!temp) {
        va_end(args);
        return -1;
    }
    
    int ret = vsprintf(temp, format, args);
    va_end(args);
    
    if (ret >= 0) {
        size_t copy_size = ret >= size ? size - 1 : ret;
        memcpy(str, temp, copy_size);
        str[copy_size] = '\0';
    }
    
    free(temp);
    return ret;
}

/* vsnprintf implementation */
int vsnprintf(char* str, size_t size, const char* format, va_list args) {
    if (size == 0) return 0;
    
    char* start = str;
    char* end = str + size - 1;  /* Leave room for null terminator */
    
    while (*format && str < end) {
        if (*format != '%') {
            *str++ = *format++;
            continue;
        }
        
        format++;  /* Skip '%' */
        
        /* Handle format specifiers */
        switch (*format) {
            case 's': {
                const char* s = va_arg(args, const char*);
                while (*s && str < end) {
                    *str++ = *s++;
                }
                break;
            }
            case 'd': {
                int num = va_arg(args, int);
                char num_buf[32];
                int len = 0;
                
                /* Convert number to string */
                if (num < 0) {
                    if (str < end) *str++ = '-';
                    num = -num;
                }
                do {
                    num_buf[len++] = '0' + (num % 10);
                    num /= 10;
                } while (num && len < 31);
                
                /* Copy in reverse */
                while (len > 0 && str < end) {
                    *str++ = num_buf[--len];
                }
                break;
            }
            case 'x': {
                unsigned int num = va_arg(args, unsigned int);
                char num_buf[32];
                int len = 0;
                
                /* Convert number to hex */
                do {
                    int digit = num & 0xF;
                    num_buf[len++] = digit < 10 ? '0' + digit : 'a' + (digit - 10);
                    num >>= 4;
                } while (num && len < 31);
                
                /* Copy in reverse */
                while (len > 0 && str < end) {
                    *str++ = num_buf[--len];
                }
                break;
            }
            default:
                if (str < end) *str++ = *format;
                break;
        }
        format++;
    }
    
    *str = '\0';  /* Null terminate */
    return str - start;
} 