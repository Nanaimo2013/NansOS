/**
 * Standard Type Definitions
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once

/* Null pointer constant */
#define NULL ((void*)0)

/* Size type */
typedef unsigned long long size_t;

/* Signed size type */
typedef long long ssize_t;

/* Pointer difference type */
typedef long long ptrdiff_t;

/* Maximum alignment requirement */
#define MAX_ALIGN 16

/* Offset of member in structure */
#define offsetof(type, member) __builtin_offsetof(type, member)

/* Wide character type */
typedef int wchar_t; 