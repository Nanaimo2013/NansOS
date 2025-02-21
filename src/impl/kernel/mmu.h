/**
 * Memory Management Unit
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>
#include <stddef.h>

/* Page size definitions */
#define PAGE_SIZE           4096
#define PAGE_TABLE_ENTRIES  512
#define PAGE_DIR_ENTRIES    512

/* Page flags */
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITABLE   (1 << 1)
#define PAGE_USER       (1 << 2)
#define PAGE_WRITETHROUGH (1 << 3)
#define PAGE_NOCACHE    (1 << 4)
#define PAGE_ACCESSED   (1 << 5)
#define PAGE_DIRTY      (1 << 6)
#define PAGE_HUGE       (1 << 7)
#define PAGE_GLOBAL     (1 << 8)

/* Memory regions */
struct MemoryRegion {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t acpi_attr;
};

/* Memory map */
struct MemoryMap {
    struct MemoryRegion* regions;
    uint32_t count;
};

/* Page table entry */
struct PageTableEntry {
    uint64_t value;
};

/* Page directory entry */
struct PageDirectoryEntry {
    uint64_t value;
};

/* Memory management functions */
void mmu_init(void);
void* mmu_alloc_page(void);
void mmu_free_page(void* page);
void mmu_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags);
void mmu_unmap_page(uint64_t virt_addr);
int mmu_is_mapped(uint64_t virt_addr);
void mmu_load_cr3(uint64_t pml4_addr);
void mmu_enable_paging(void);
void mmu_set_kernel_stack(uint64_t stack);

/* Memory region management */
void mmu_add_memory_region(uint64_t base, uint64_t length, uint32_t type);
struct MemoryRegion* mmu_get_memory_regions(uint32_t* count);
uint64_t mmu_get_total_memory(void);
uint64_t mmu_get_available_memory(void);

/* Virtual memory management */
void* vmm_alloc(size_t size);
void vmm_free(void* ptr);
void* vmm_alloc_aligned(size_t size, size_t alignment);
void vmm_init(void); 