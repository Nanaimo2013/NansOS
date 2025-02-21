/**
 * Main Test Runner
 * NansOS Test Suite
 * Copyright (c) 2025 NansStudios
 */

#include "test_framework.h"
#include "../src/intf/print.h"
#include "../src/impl/kernel/asm_utils.h"
#include "../src/impl/drivers/serial/serial.h"

/* External test suite declarations */
extern struct TestSuite interrupt_test_suite;
extern struct TestSuite driver_test_suite;
extern struct TestSuite memory_test_suite;

/* Test suites array */
static struct TestSuite* test_suites[] = {
    &memory_test_suite,    /* Run memory tests first */
    &interrupt_test_suite, /* Then interrupts */
    &driver_test_suite     /* Finally device drivers */
};

#define NUM_TEST_SUITES (sizeof(test_suites) / sizeof(struct TestSuite*))

/* Calculate total number of tests */
static size_t get_total_tests(void) {
    size_t total = 0;
    for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
        total += test_suites[i]->test_count;
    }
    return total;
}

/* Main test entry point */
void run_tests(void) {
    struct TestStats total_stats = {0, 0, 0};
    int current_test = 0;
    size_t total_tests = get_total_tests();
    
    /* Initialize serial port for test output */
    serial_init(COM1_PORT, SERIAL_BAUD_115200);
    
    /* Print header */
    print_set_color(PRINT_COLOR_CYAN, PRINT_COLOR_BLACK);
    print_str("\n=== NansOS Test Suite ===\n\n");
    serial_write_string(COM1_PORT, "\n=== NansOS Test Suite ===\n\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    
    /* Run each test suite */
    for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
        struct TestStats suite_stats = run_test_suite(test_suites[i], &current_test, total_tests);
        
        total_stats.total_tests += suite_stats.total_tests;
        total_stats.passed_tests += suite_stats.passed_tests;
        total_stats.failed_tests += suite_stats.failed_tests;
    }
    
    /* Print final summary */
    print_set_color(PRINT_COLOR_CYAN, PRINT_COLOR_BLACK);
    print_str("\n=== Final Test Summary ===\n");
    serial_write_string(COM1_PORT, "\n=== Final Test Summary ===\n");
    
    char buffer[64];
    sprintf(buffer, "Total Tests: %d\n", total_stats.total_tests);
    print_str(buffer);
    serial_write_string(COM1_PORT, buffer);
    
    print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
    sprintf(buffer, "Passed: %d\n", total_stats.passed_tests);
    print_str(buffer);
    serial_write_string(COM1_PORT, buffer);
    
    print_set_color(PRINT_COLOR_RED, PRINT_COLOR_BLACK);
    sprintf(buffer, "Failed: %d\n", total_stats.failed_tests);
    print_str(buffer);
    serial_write_string(COM1_PORT, buffer);
    
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    sprintf(buffer, "Pass Rate: %d%%\n", (total_stats.passed_tests * 100) / total_stats.total_tests);
    print_str(buffer);
    serial_write_string(COM1_PORT, buffer);
    
    /* Final status and wait for input if there were failures */
    if (total_stats.failed_tests > 0) {
        print_set_color(PRINT_COLOR_RED, PRINT_COLOR_BLACK);
        wait_for_input("\nSome tests failed!");
    } else {
        print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
        print_str("\nAll tests passed successfully!\n");
        serial_write_string(COM1_PORT, "\nAll tests passed successfully!\n");
    }
    
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    serial_write_string(COM1_PORT, "\nTest execution complete.\n");
} 