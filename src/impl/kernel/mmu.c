/**
 * Memory Management Unit Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "mmu.h"
#include "asm_utils.h"
#include "../../intf/print.h"
#include <string.h>

/* Simple memory allocator state */
static uint64_t next_free_page = 0x400000;  /* Start after boot loader's huge pages */
static uint64_t heap_start = 0x400000;      /* Start heap after boot loader's pages */
static uint64_t heap_current = 0x400000;    /* Current heap position */
static uint64_t heap_end = 0x800000;        /* End at 8MB initially */

/* Initialize memory management */
void mmu_init(void) {
    /* Keep the boot loader's page tables - they already map the first 2MB */
    print_str("[MMU] Using boot loader's page tables\n");
}

/* Initialize virtual memory manager */
void vmm_init(void) {
    heap_current = heap_start;
    print_str("[VMM] Basic heap initialized\n");
}

/* Allocate a physical page */
void* mmu_alloc_page(void) {
    void* page = (void*)next_free_page;
    next_free_page += PAGE_SIZE;
    memset(page, 0, PAGE_SIZE);
    return page;
}

/* Free a physical page */
void mmu_free_page(void* page) {
    /* No-op for now */
    (void)page;
}

/* Map a virtual page to a physical page */
void mmu_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags) {
    /* Only map pages beyond the boot loader's huge pages */
    if (virt_addr < 0x400000) {
        return;  /* These are already mapped by the boot loader */
    }

    uint64_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint64_t pdp_idx = (virt_addr >> 30) & 0x1FF;
    uint64_t pd_idx = (virt_addr >> 21) & 0x1FF;
    uint64_t pt_idx = (virt_addr >> 12) & 0x1FF;
    
    uint64_t* pml4 = (uint64_t*)asm_read_cr3();
    
    /* Ensure PDP exists */
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t pdp = (uint64_t)mmu_alloc_page();
        memset((void*)pdp, 0, PAGE_SIZE);
        pml4[pml4_idx] = pdp | flags;
    }
    uint64_t* pdp = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);
    
    /* Ensure PD exists */
    if (!(pdp[pdp_idx] & PAGE_PRESENT)) {
        uint64_t pd = (uint64_t)mmu_alloc_page();
        memset((void*)pd, 0, PAGE_SIZE);
        pdp[pdp_idx] = pd | flags;
    }
    uint64_t* pd = (uint64_t*)(pdp[pdp_idx] & ~0xFFF);
    
    /* Check if this is already mapped as a huge page */
    if (pd[pd_idx] & PAGE_HUGE) {
        return;  /* Don't modify huge page mappings */
    }
    
    /* Ensure PT exists */
    if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t pt = (uint64_t)mmu_alloc_page();
        memset((void*)pt, 0, PAGE_SIZE);
        pd[pd_idx] = pt | flags;
    }
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
    
    /* Map the page */
    pt[pt_idx] = phys_addr | flags;
}

/* Check if a virtual address is mapped */
int mmu_is_mapped(uint64_t virt_addr) {
    uint64_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint64_t pdp_idx = (virt_addr >> 30) & 0x1FF;
    uint64_t pd_idx = (virt_addr >> 21) & 0x1FF;
    uint64_t pt_idx = (virt_addr >> 12) & 0x1FF;
    
    uint64_t* pml4 = (uint64_t*)asm_read_cr3();
    
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) return 0;
    uint64_t* pdp = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);
    
    if (!(pdp[pdp_idx] & PAGE_PRESENT)) return 0;
    uint64_t* pd = (uint64_t*)(pdp[pdp_idx] & ~0xFFF);
    
    /* Check for huge page */
    if (pd[pd_idx] & PAGE_HUGE) return 1;
    
    if (!(pd[pd_idx] & PAGE_PRESENT)) return 0;
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
    
    return (pt[pt_idx] & PAGE_PRESENT) != 0;
}

/* Unmap a virtual page */
void mmu_unmap_page(uint64_t virt_addr) {
    uint64_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint64_t pdp_idx = (virt_addr >> 30) & 0x1FF;
    uint64_t pd_idx = (virt_addr >> 21) & 0x1FF;
    uint64_t pt_idx = (virt_addr >> 12) & 0x1FF;
    
    uint64_t* pml4 = (uint64_t*)0x1000;
    
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) return;
    uint64_t* pdp = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);
    
    if (!(pdp[pdp_idx] & PAGE_PRESENT)) return;
    uint64_t* pd = (uint64_t*)(pdp[pdp_idx] & ~0xFFF);
    
    if (!(pd[pd_idx] & PAGE_PRESENT)) return;
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
    
    pt[pt_idx] = 0;
}

/* Load CR3 with a new page table */
void mmu_load_cr3(uint64_t pml4_addr) {
    asm_write_cr3(pml4_addr);
}

/* Enable paging */
void mmu_enable_paging(void) {
    uint64_t cr0 = asm_read_cr0();
    cr0 |= 0x80000000;
    asm_write_cr0(cr0);
}

/* Set kernel stack */
void mmu_set_kernel_stack(uint64_t stack) {
    /* Use the boot loader's stack mapping */
    print_str("[MMU] Using boot loader's stack mapping\n");
}

/* Simplified memory region tracking */
void mmu_add_memory_region(uint64_t base, uint64_t length, uint32_t type) {
    /* No-op for now */
    (void)base;
    (void)length;
    (void)type;
}

struct MemoryRegion* mmu_get_memory_regions(uint32_t* count) {
    *count = 0;
    return NULL;
}

uint64_t mmu_get_total_memory(void) {
    return 8 * 1024 * 1024;  /* Report 8MB */
}

uint64_t mmu_get_available_memory(void) {
    return 6 * 1024 * 1024;  /* Report 6MB available */
} 