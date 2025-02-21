/**
 * Assembly Utilities
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* GCC/Clang inline assembly */
#if defined(__GNUC__) || defined(__clang__)
    #define ASM_INLINE __asm__ __volatile__
    #define HAVE_INLINE_ASM 1

/* MSVC - use intrinsics for x64, inline assembly for x86 */
#elif defined(_MSC_VER)
    #include <intrin.h>
    #ifdef _M_X64
        #define HAVE_INTRINSICS 1
    #else
        #define ASM_INLINE __asm
        #define HAVE_INLINE_ASM 1
    #endif
#else
    #error "Unsupported compiler - this code requires GCC, Clang, or MSVC"
#endif

/* CPUID operation */
static inline void asm_cpuid(uint32_t leaf, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
#if defined(HAVE_INTRINSICS)
    int cpu_info[4];
    __cpuid(cpu_info, leaf);
    *eax = cpu_info[0];
    *ebx = cpu_info[1];
    *ecx = cpu_info[2];
    *edx = cpu_info[3];
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (leaf)
        : "memory"
    );
#endif
}

/* Read MSR operation */
static inline uint64_t asm_rdmsr(uint32_t msr) {
#if defined(HAVE_INTRINSICS)
    return __readmsr(msr);
#elif defined(HAVE_INLINE_ASM)
    uint32_t low, high;
    ASM_INLINE (
        "rdmsr"
        : "=a" (low), "=d" (high)
        : "c" (msr)
        : "memory"
    );
    return ((uint64_t)high << 32) | low;
#else
    return 0;
#endif
}

/* Write MSR operation */
static inline void asm_wrmsr(uint32_t msr, uint64_t value) {
#if defined(HAVE_INTRINSICS)
    __writemsr(msr, value);
#elif defined(HAVE_INLINE_ASM)
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    ASM_INLINE (
        "wrmsr"
        :
        : "c" (msr), "a" (low), "d" (high)
        : "memory"
    );
#endif
}

/* Port I/O operations */
static inline void asm_outb(uint16_t port, uint8_t value) {
#if defined(HAVE_INTRINSICS)
    __outbyte(port, value);
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE (
        "mov dx, %1\n\t"
        "mov al, %0\n\t"
        "out dx, al"
        :
        : "r" (value), "r" (port)
        : "dx", "al"
    );
#endif
}

static inline uint8_t asm_inb(uint16_t port) {
#if defined(HAVE_INTRINSICS)
    return __inbyte(port);
#elif defined(HAVE_INLINE_ASM)
    uint8_t value;
    ASM_INLINE (
        "mov dx, %1\n\t"
        "in al, dx\n\t"
        "mov %0, al"
        : "=r" (value)
        : "r" (port)
        : "dx", "al"
    );
    return value;
#endif
}

static inline void asm_outw(uint16_t port, uint16_t value) {
#if defined(HAVE_INTRINSICS)
    __outword(port, value);
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE (
        "mov dx, %1\n\t"
        "mov ax, %0\n\t"
        "out dx, ax"
        :
        : "r" (value), "r" (port)
        : "dx", "ax"
    );
#endif
}

static inline uint16_t asm_inw(uint16_t port) {
#if defined(HAVE_INTRINSICS)
    return __inword(port);
#elif defined(HAVE_INLINE_ASM)
    uint16_t value;
    ASM_INLINE (
        "mov dx, %1\n\t"
        "in ax, dx\n\t"
        "mov %0, ax"
        : "=r" (value)
        : "r" (port)
        : "dx", "ax"
    );
    return value;
#endif
}

static inline void asm_outl(uint16_t port, uint32_t value) {
#if defined(HAVE_INTRINSICS)
    __outdword(port, value);
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE (
        "outl %%eax, %%dx"
        :
        : "d" (port), "a" (value)
    );
#endif
}

static inline uint32_t asm_inl(uint16_t port) {
#if defined(HAVE_INTRINSICS)
    return __indword(port);
#elif defined(HAVE_INLINE_ASM)
    uint32_t value;
    ASM_INLINE (
        "inl %%dx, %%eax"
        : "=a" (value)
        : "d" (port)
    );
    return value;
#endif
}

/* Additional x86_64 specific operations */
#ifdef __x86_64__
static inline void asm_cli(void) {
#if defined(HAVE_INTRINSICS)
    _disable();
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE ("cli" ::: "memory");
#endif
}

static inline void asm_sti(void) {
#if defined(HAVE_INTRINSICS)
    _enable();
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE ("sti" ::: "memory");
#endif
}

static inline void asm_hlt(void) {
#if defined(HAVE_INTRINSICS)
    __halt();
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE ("hlt" ::: "memory");
#endif
}

static inline uint64_t asm_read_cr0(void) {
#if defined(HAVE_INTRINSICS)
    return __readcr0();
#elif defined(HAVE_INLINE_ASM)
    uint64_t cr0;
    ASM_INLINE ("mov %%cr0, %0" : "=r" (cr0) :: "memory");
    return cr0;
#else
    return 0;
#endif
}

static inline void asm_write_cr0(uint64_t value) {
#if defined(HAVE_INTRINSICS)
    __writecr0(value);
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE ("mov %0, %%cr0" :: "r" (value) : "memory");
#endif
}

static inline uint64_t asm_read_cr2(void) {
#if defined(HAVE_INTRINSICS)
    return __readcr2();
#elif defined(HAVE_INLINE_ASM)
    uint64_t cr2;
    ASM_INLINE ("mov %%cr2, %0" : "=r" (cr2) :: "memory");
    return cr2;
#else
    return 0;
#endif
}

static inline uint64_t asm_read_cr3(void) {
#if defined(HAVE_INTRINSICS)
    return __readcr3();
#elif defined(HAVE_INLINE_ASM)
    uint64_t cr3;
    ASM_INLINE ("mov %%cr3, %0" : "=r" (cr3) :: "memory");
    return cr3;
#else
    return 0;
#endif
}

static inline void asm_write_cr3(uint64_t value) {
#if defined(HAVE_INTRINSICS)
    __writecr3(value);
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE ("mov %0, %%cr3" :: "r" (value) : "memory");
#endif
}

struct IDTPointer;  /* Forward declaration */

static inline void asm_lidt(struct IDTPointer* ptr) {
#if defined(HAVE_INTRINSICS)
    __lidt(ptr);
#elif defined(HAVE_INLINE_ASM)
    ASM_INLINE ("lidt [%0]" :: "r"(ptr) : "memory");
#endif
}
#endif /* __x86_64__ */ 