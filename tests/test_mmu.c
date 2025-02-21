/**
 * Memory Management Unit Tests
 * NansOS Test Suite
 * Copyright (c) 2025 NansStudios
 */

#include "test_framework.h"
#include "../src/impl/kernel/mmu.h"
#include "../src/intf/print.h"

/* Test page allocation */
static struct TestResult test_page_allocation(void) {
    void* page = mmu_alloc_page();
    TEST_ASSERT_NOT_NULL(page, "Failed to allocate page");
    
    /* Check page alignment */
    TEST_ASSERT(((uint64_t)page & 0xFFF) == 0, "Page not properly aligned");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test virtual memory mapping */
static struct TestResult test_virtual_mapping(void) {
    uint64_t phys_addr = (uint64_t)mmu_alloc_page();
    uint64_t virt_addr = 0x400000; /* 4MB mark */
    
    TEST_ASSERT_NOT_NULL((void*)phys_addr, "Failed to allocate physical page");
    
    /* Map the page */
    mmu_map_page(phys_addr, virt_addr, PAGE_PRESENT | PAGE_WRITABLE);
    
    /* Verify mapping */
    TEST_ASSERT(mmu_is_mapped(virt_addr), "Virtual address not mapped");
    
    /* Unmap the page */
    mmu_unmap_page(virt_addr);
    
    /* Verify unmapping */
    TEST_ASSERT(!mmu_is_mapped(virt_addr), "Virtual address still mapped after unmap");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test kernel heap expansion */
static struct TestResult test_kernel_heap(void) {
    /* Initialize virtual memory */
    vmm_init();
    
    /* Try to write to the heap */
    uint64_t* ptr = (uint64_t*)0x100000000; /* Start of kernel heap */
    *ptr = 0xDEADBEEF;
    
    TEST_ASSERT(*ptr == 0xDEADBEEF, "Failed to write to kernel heap");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test kernel stack setup */
static struct TestResult test_kernel_stack(void) {
    uint64_t stack_addr = 0x300000; /* 3MB mark */
    
    /* Set up kernel stack */
    mmu_set_kernel_stack(stack_addr);
    
    /* Verify stack pages are mapped */
    TEST_ASSERT(mmu_is_mapped(stack_addr - PAGE_SIZE), "Stack page not mapped");
    TEST_ASSERT(mmu_is_mapped(stack_addr - (2 * PAGE_SIZE)), "Stack page not mapped");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Memory test suite */
static TestFunction mmu_tests[] = {
    test_page_allocation,
    test_virtual_mapping,
    test_kernel_heap,
    test_kernel_stack
};

struct TestSuite mmu_test_suite = {
    .name = "Memory Management Tests",
    .tests = mmu_tests,
    .test_count = sizeof(mmu_tests) / sizeof(TestFunction)
}; 