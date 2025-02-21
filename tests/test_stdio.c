/**
 * Standard I/O Tests
 * NansOS Test Suite
 * Copyright (c) 2025 NansStudios
 */

#include "test_framework.h"
#include "../src/intf/stdio.h"
#include <string.h>

/* Helper function for vsprintf test */
static int test_vsprintf_helper(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(buffer, format, args);
    va_end(args);
    return result;
}

/* Helper function for vsnprintf test */
static int test_vsnprintf_helper(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

/* Test sprintf basic functionality */
static struct TestResult test_sprintf_basic(void) {
    char buffer[256];
    int result = sprintf(buffer, "Hello %s", "World");
    
    TEST_ASSERT_EQUAL(11, result, "Incorrect return value from sprintf");
    TEST_ASSERT(strcmp(buffer, "Hello World") == 0, "Incorrect string output");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test sprintf with numbers */
static struct TestResult test_sprintf_numbers(void) {
    char buffer[256];
    int result = sprintf(buffer, "Int: %d, Hex: 0x%x", 42, 0xDEAD);
    
    TEST_ASSERT_EQUAL(20, result, "Incorrect return value from sprintf");
    TEST_ASSERT(strcmp(buffer, "Int: 42, Hex: 0xdead") == 0, "Incorrect number formatting");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test snprintf bounds checking */
static struct TestResult test_snprintf_bounds(void) {
    char buffer[10];
    int result = snprintf(buffer, sizeof(buffer), "This is a long string");
    
    TEST_ASSERT_EQUAL(9, strlen(buffer), "Buffer overflow occurred");
    TEST_ASSERT(buffer[9] == '\0', "String not null-terminated");
    TEST_ASSERT(result > 9, "Incorrect total length returned");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test vsprintf functionality */
static struct TestResult test_vsprintf(void) {
    char buffer[256];
    int result = test_vsprintf_helper(buffer, "Test %s %d", "string", 42);
    
    TEST_ASSERT_EQUAL(15, result, "Incorrect return value from vsprintf");
    TEST_ASSERT(strcmp(buffer, "Test string 42") == 0, "Incorrect string output");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* Test vsnprintf bounds checking */
static struct TestResult test_vsnprintf(void) {
    char buffer[10];
    int result = test_vsnprintf_helper(buffer, sizeof(buffer), "Long string %s", "test");
    
    TEST_ASSERT_EQUAL(9, strlen(buffer), "Buffer overflow occurred");
    TEST_ASSERT(buffer[9] == '\0', "String not null-terminated");
    TEST_ASSERT(result > 9, "Incorrect total length returned");
    
    return (struct TestResult){__func__, 1, NULL};
}

/* StdIO test suite */
static TestFunction stdio_tests[] = {
    test_sprintf_basic,
    test_sprintf_numbers,
    test_snprintf_bounds,
    test_vsprintf,
    test_vsnprintf
};

struct TestSuite stdio_test_suite = {
    .name = "Standard I/O Tests",
    .tests = stdio_tests,
    .test_count = sizeof(stdio_tests) / sizeof(TestFunction)
}; 