/**
 * Standard Library Functions
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stddef.h>

/* Memory allocation */
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

/* Conversion functions */
int atoi(const char* nptr);
long atol(const char* nptr);
long long atoll(const char* nptr);

/* Random number generation */
int rand(void);
void srand(unsigned int seed);

/* Sorting and searching */
void qsort(void* base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*));
void* bsearch(const void* key, const void* base,
              size_t nmemb, size_t size,
              int (*compar)(const void*, const void*)); 