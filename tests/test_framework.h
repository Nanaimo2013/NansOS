/**
 * NansOS Test Framework
 * Copyright (c) 2025 NansStudios
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdint.h>
#include "../src/intf/print.h"
#include "../src/intf/stdio.h"
#include "../src/impl/drivers/serial/serial.h"
#include "../src/impl/drivers/keyboard/keyboard.h"

/* Define boolean type */
typedef enum { false = 0, true = 1 } bool;

/* Progress bar configuration */
#define PROGRESS_BAR_WIDTH 50
#define PROGRESS_CHAR "█"
#define EMPTY_CHAR "░"

/* Test result structure */
struct TestResult {
    const char* test_name;
    bool passed;
    const char* error_message;
};

/* Test statistics */
struct TestStats {
    int total_tests;
    int passed_tests;
    int failed_tests;
};

/* Helper function to write integers to serial */
static void serial_write_int(int value) {
    char buffer[32];
    sprintf(buffer, "%d", value);
    serial_write_string(COM1_PORT, buffer);
}

/* Progress bar functions */
static void print_progress_bar(int progress, int total) {
    int position = (progress * PROGRESS_BAR_WIDTH) / total;
    
    print_str("[");
    serial_write_string(COM1_PORT, "[");
    
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < position) {
            print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
            print_str(PROGRESS_CHAR);
            serial_write_string(COM1_PORT, PROGRESS_CHAR);
        } else {
            print_set_color(PRINT_COLOR_DARK_GRAY, PRINT_COLOR_BLACK);
            print_str(EMPTY_CHAR);
            serial_write_string(COM1_PORT, EMPTY_CHAR);
        }
    }
    
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    print_str("] ");
    serial_write_string(COM1_PORT, "] ");
    
    char buffer[8];
    sprintf(buffer, "%d%%", (progress * 100) / total);
    print_str(buffer);
    serial_write_string(COM1_PORT, buffer);
    print_str("\n");
    serial_write_string(COM1_PORT, "\n");
}

/* Wait for user input */
static void wait_for_input(const char* message) {
    static bool keyboard_initialized = false;
    
    /* Initialize keyboard if not already done */
    if (!keyboard_initialized) {
        keyboard_init();
        keyboard_initialized = true;
    }
    
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str(message);
    print_str(" (Press Enter to continue)\n");
    serial_write_string(COM1_PORT, message);
    serial_write_string(COM1_PORT, " (Press Enter to continue)\n");
    
    /* Wait for Enter key */
    bool enter_pressed = false;
    while (!enter_pressed) {
        uint8_t scancode = keyboard_read_scan_code();
        if (scancode == 0x1C) { /* Enter key scancode */
            enter_pressed = true;
        }
        /* Small delay */
        for (volatile int i = 0; i < 1000000; i++) {}
    }
    
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
}

/* Test macros */
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            return (struct TestResult){__func__, false, message}; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            return (struct TestResult){__func__, false, message}; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr, message) \
    do { \
        if ((ptr) == NULL) { \
            return (struct TestResult){__func__, false, message}; \
        } \
    } while(0)

/* Test function type */
typedef struct TestResult (*TestFunction)(void);

/* Test suite structure */
struct TestSuite {
    const char* name;
    TestFunction* tests;
    size_t test_count;
};

/* Print test results */
static void print_test_result(struct TestResult result, int current, int total) {
    char buffer[256];
    
    /* Update progress bar */
    print_progress_bar(current, total);
    
    if (result.passed) {
        print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
        print_str("[PASS] ");
        sprintf(buffer, "[PASS] %s\n", result.test_name);
    } else {
        print_set_color(PRINT_COLOR_RED, PRINT_COLOR_BLACK);
        print_str("[FAIL] ");
        sprintf(buffer, "[FAIL] %s: %s\n", result.test_name, result.error_message);
    }
    
    print_str(result.test_name);
    if (!result.passed && result.error_message) {
        print_str(": ");
        print_str(result.error_message);
    }
    print_str("\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    
    serial_write_string(COM1_PORT, buffer);
}

/* Run test suite and return statistics */
static struct TestStats run_test_suite(struct TestSuite* suite, int* current_test, int total_tests) {
    struct TestStats stats = {0, 0, 0};
    bool suite_has_failures = false;
    
    /* Print suite header */
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("\nRunning Suite: ");
    print_str(suite->name);
    print_str("\n");
    serial_write_string(COM1_PORT, "\nRunning Suite: ");
    serial_write_string(COM1_PORT, suite->name);
    serial_write_string(COM1_PORT, "\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    
    /* Run tests */
    for (size_t i = 0; i < suite->test_count; i++) {
        struct TestResult result = suite->tests[i]();
        (*current_test)++;
        
        print_test_result(result, *current_test, total_tests);
        
        stats.total_tests++;
        if (result.passed) {
            stats.passed_tests++;
        } else {
            stats.failed_tests++;
            suite_has_failures = true;
        }
    }
    
    /* Print suite summary */
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("\nSuite Summary: ");
    print_str(suite->name);
    print_str("\n");
    print_str("Passed: ");
    char buffer[32];
    sprintf(buffer, "%d/%d\n", stats.passed_tests, stats.total_tests);
    print_str(buffer);
    serial_write_string(COM1_PORT, buffer);
    
    /* If suite had failures, wait for user input */
    if (suite_has_failures) {
        wait_for_input("\nSuite had failures!");
    }
    
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    
    return stats;
}

#endif /* TEST_FRAMEWORK_H */ 