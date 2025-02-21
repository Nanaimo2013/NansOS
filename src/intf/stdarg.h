/**
 * Variable Arguments
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once

/* Compiler-specific implementations */
#if defined(__GNUC__) || defined(__clang__)
    /* GCC and Clang use the same built-in types */
    typedef __builtin_va_list va_list;
    #define va_start(v,l) __builtin_va_start(v,l)
    #define va_end(v)     __builtin_va_end(v)
    #define va_arg(v,l)   __builtin_va_arg(v,l)
    #define va_copy(d,s)  __builtin_va_copy(d,s)
#elif defined(_MSC_VER)
    /* MSVC implementation */
    typedef char* va_list;
    #define va_start(ap,v) ((void)(ap = (va_list)&v + sizeof(v)))
    #define va_arg(ap,t)   (*(t*)((ap += sizeof(t)) - sizeof(t)))
    #define va_end(ap)     ((void)(ap = (va_list)0))
    #define va_copy(d,s)   ((void)(d = s))
#else
    /* Fallback to GCC built-ins if no specific implementation */
    typedef __builtin_va_list va_list;
    #define va_start(v,l) __builtin_va_start(v,l)
    #define va_end(v)     __builtin_va_end(v)
    #define va_arg(v,l)   __builtin_va_arg(v,l)
    #define va_copy(d,s)  __builtin_va_copy(d,s)
#endif 