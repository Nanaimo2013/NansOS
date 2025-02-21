/**
 * Standard I/O Functions
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdarg.h>
#include <stddef.h>

/* Standard I/O functions */
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, size_t size, const char* format, ...);
int vsprintf(char* str, const char* format, va_list args);
int vsnprintf(char* str, size_t size, const char* format, va_list args);