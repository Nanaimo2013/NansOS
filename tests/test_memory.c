/**
 * Memory Management Tests
 * NansOS Test Suite
 * Copyright (c) 2025 NansStudios
 */

#include "test_framework.h"
#include "../src/impl/kernel/mmu.h"

/* Test page table initialization */
static struct TestResult test_page_table_init(void) {
    mmu_init();
    
    /* Verify page directory is set up */
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    TEST_ASSERT(cr3 != 0, "Page directory not initialized");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test physical memory allocation */
static struct TestResult test_physical_alloc(void) {
    /* Allocate a physical page */
    uint64_t phys_addr = mmu_alloc_page();
    TEST_ASSERT(phys_addr != 0, "Physical page allocation failed");
    
    /* Verify page alignment */
    TEST_ASSERT((phys_addr & 0xFFF) == 0, "Physical page not aligned");
    
    /* Free the page */
    mmu_free_page(phys_addr);
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test virtual memory mapping */
static struct TestResult test_virtual_mapping(void) {
    uint64_t phys_addr = mmu_alloc_page();
    uint64_t virt_addr = 0x400000; /* 4MB mark */
    
    /* Map the page */
    int result = mmu_map_page(virt_addr, phys_addr, PAGE_PRESENT | PAGE_WRITE);
    TEST_ASSERT(result == 0, "Page mapping failed");
    
    /* Verify mapping */
    uint64_t mapped_phys = mmu_get_physical_address(virt_addr);
    TEST_ASSERT(mapped_phys == phys_addr, "Physical address mismatch");
    
    /* Cleanup */
    mmu_unmap_page(virt_addr);
    mmu_free_page(phys_addr);
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test kernel stack setup */
static struct TestResult test_kernel_stack(void) {
    uint64_t stack_ptr = 0;
    
    /* Get current stack pointer */
    asm volatile("mov %%rsp, %0" : "=r"(stack_ptr));
    
    /* Verify stack is in kernel space */
    TEST_ASSERT(stack_ptr >= KERNEL_VIRTUAL_BASE, "Stack not in kernel space");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Memory management test suite */
static TestFunction memory_tests[] = {
    test_page_table_init,
    test_physical_alloc,
    test_virtual_mapping,
    test_kernel_stack
};

struct TestSuite memory_test_suite = {
    .name = "Memory Management Tests",
    .tests = memory_tests,
    .test_count = sizeof(memory_tests) / sizeof(TestFunction)
}; 