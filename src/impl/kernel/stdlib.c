/**
 * Standard Library Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "../../intf/stdlib.h"
#include "../../intf/string.h"
#include "mmu.h"

/* Memory block header */
struct MemBlock {
    size_t size;
    int is_free;
    struct MemBlock* next;
};

/* Memory allocator state */
static struct MemBlock* heap_start = NULL;
static const size_t MIN_ALLOC = 4096;  /* Minimum allocation size (4KB) */

/* Initialize a new block of memory */
static struct MemBlock* init_block(void* addr, size_t size) {
    struct MemBlock* block = (struct MemBlock*)addr;
    block->size = size - sizeof(struct MemBlock);
    block->is_free = 1;
    block->next = NULL;
    return block;
}

/* Find a free block of sufficient size */
static struct MemBlock* find_free_block(struct MemBlock** last, size_t size) {
    struct MemBlock* current = heap_start;
    while (current && !(current->is_free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

/* Split a block if it's too large */
static void split_block(struct MemBlock* block, size_t size) {
    if (block->size >= size + sizeof(struct MemBlock) + 32) {
        struct MemBlock* new_block = (struct MemBlock*)((char*)block + sizeof(struct MemBlock) + size);
        new_block->size = block->size - size - sizeof(struct MemBlock);
        new_block->is_free = 1;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

void* malloc(size_t size) {
    if (size == 0) return NULL;

    /* Align size to 8 bytes */
    size = (size + 7) & ~7;

    struct MemBlock* block;
    struct MemBlock* last = NULL;

    /* First allocation */
    if (!heap_start) {
        size_t alloc_size = size + sizeof(struct MemBlock);
        if (alloc_size < MIN_ALLOC) {
            alloc_size = MIN_ALLOC;
        }
        
        block = (struct MemBlock*)mmu_alloc_page();
        if (!block) return NULL;
        
        heap_start = init_block(block, alloc_size);
        block = heap_start;
    } else {
        /* Find a free block */
        block = find_free_block(&last, size);
        
        /* No free block found, allocate new memory */
        if (!block) {
            size_t alloc_size = size + sizeof(struct MemBlock);
            if (alloc_size < MIN_ALLOC) {
                alloc_size = MIN_ALLOC;
            }
            
            block = (struct MemBlock*)mmu_alloc_page();
            if (!block) return NULL;
            
            block = init_block(block, alloc_size);
            last->next = block;
        }
    }

    /* Split block if it's too large */
    split_block(block, size);

    /* Mark block as used */
    block->is_free = 0;

    /* Return pointer to usable memory */
    return (void*)(block + 1);
}

void free(void* ptr) {
    if (!ptr) return;

    /* Get block header */
    struct MemBlock* block = (struct MemBlock*)ptr - 1;
    block->is_free = 1;

    /* Merge with next block if free */
    if (block->next && block->next->is_free) {
        block->size += sizeof(struct MemBlock) + block->next->size;
        block->next = block->next->next;
    }
}

void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* ptr = malloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    struct MemBlock* block = (struct MemBlock*)ptr - 1;
    if (block->size >= size) {
        /* Block is large enough */
        split_block(block, size);
        return ptr;
    }

    /* Need to allocate new block */
    void* new_ptr = malloc(size);
    if (!new_ptr) return NULL;

    /* Copy old data */
    memcpy(new_ptr, ptr, block->size);
    free(ptr);

    return new_ptr;
} 