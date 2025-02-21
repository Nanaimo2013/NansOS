/**
 * Interrupt Handling Tests
 * NansOS Test Suite
 * Copyright (c) 2025 NansStudios
 */

#include "test_framework.h"
#include "../src/impl/kernel/idt.h"
#include "../src/impl/kernel/isr.h"
#include "../src/impl/drivers/pic/pic.h"

/* Test IDT initialization */
static struct TestResult test_idt_initialization(void) {
    /* Initialize IDT */
    idt_init();
    
    /* Verify IDT pointer is set */
    struct IDTPointer* idt_ptr = (struct IDTPointer*)0;
    asm volatile("sidt %0" : "=m"(*idt_ptr));
    
    TEST_ASSERT(idt_ptr->limit == (256 * sizeof(struct IDTEntry) - 1), "IDT limit incorrect");
    TEST_ASSERT(idt_ptr->base != 0, "IDT base not set");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test interrupt enabling/disabling */
static struct TestResult test_interrupt_control(void) {
    uint64_t rflags;
    
    /* Test interrupt enabling */
    idt_enable_interrupts();
    asm volatile("pushfq; pop %0" : "=r"(rflags));
    TEST_ASSERT(rflags & (1 << 9), "Interrupts not enabled");
    
    /* Test interrupt disabling */
    idt_disable_interrupts();
    asm volatile("pushfq; pop %0" : "=r"(rflags));
    TEST_ASSERT(!(rflags & (1 << 9)), "Interrupts not disabled");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test PIC initialization */
static struct TestResult test_pic_initialization(void) {
    /* Initialize PIC */
    pic_init();
    
    /* Read PIC masks */
    uint8_t master_mask = port_byte_in(PIC1_DATA);
    uint8_t slave_mask = port_byte_in(PIC2_DATA);
    
    /* Verify timer, keyboard, and cascade are enabled */
    TEST_ASSERT((master_mask & 0x07) == 0, "Required interrupts not enabled");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test interrupt handler registration */
static struct TestResult test_handler_registration(void) {
    /* Set a test handler */
    idt_set_entry(0x20, (uint64_t)isr_timer_handler, 0x8E);
    
    /* Verify entry is set */
    struct IDTEntry* idt = (struct IDTEntry*)0;
    asm volatile("sidt %0" : "=m"(*((struct IDTPointer*)&idt)));
    
    TEST_ASSERT(idt[0x20].isr_low != 0, "Handler not registered");
    TEST_ASSERT(idt[0x20].attributes == 0x8E, "Handler attributes incorrect");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Interrupt test suite */
static TestFunction interrupt_tests[] = {
    test_idt_initialization,
    test_interrupt_control,
    test_pic_initialization,
    test_handler_registration
};

struct TestSuite interrupt_test_suite = {
    .name = "Interrupt Handling Tests",
    .tests = interrupt_tests,
    .test_count = sizeof(interrupt_tests) / sizeof(TestFunction)
}; 