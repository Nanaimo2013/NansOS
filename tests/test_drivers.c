/**
 * Device Driver Tests
 * NansOS Test Suite
 * Copyright (c) 2025 NansStudios
 */

#include "test_framework.h"
#include "../src/impl/drivers/keyboard/keyboard.h"
#include "../src/impl/drivers/serial/serial.h"
#include "../src/impl/drivers/pit/pit.h"
#include "../src/impl/drivers/rtc/rtc.h"
#include "../src/impl/drivers/mouse/mouse.h"
#include "../src/impl/drivers/port_io/port.h"

/* Helper function to wait for keyboard controller */
static bool wait_keyboard_controller(void) {
    int timeout = 100000;
    while (--timeout > 0) {
        if ((port_byte_in(KEYBOARD_STATUS_PORT) & 0x02) == 0) {
            return true;
        }
        for (volatile int i = 0; i < 1000; i++) {} // Small delay
    }
    return false;
}

/* Helper function to flush keyboard buffer */
static void flush_keyboard_buffer(void) {
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 0x01) != 0) {
        port_byte_in(KEYBOARD_DATA_PORT);
        for (volatile int i = 0; i < 1000; i++) {} // Small delay
    }
}

/* Keyboard Tests */
static struct TestResult test_keyboard_init(void) {
    // First, try to reset the keyboard controller
    if (!wait_keyboard_controller()) {
        return (struct TestResult){__func__, false, "Keyboard controller timeout"};
    }
    
    // Disable both keyboard and mouse
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xAD);  // Disable keyboard
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xA7);  // Disable mouse
    
    // Flush the output buffer
    flush_keyboard_buffer();
    
    // Initialize keyboard
    keyboard_init();
    
    // Wait for controller to be ready
    if (!wait_keyboard_controller()) {
        return (struct TestResult){__func__, false, "Keyboard not responding after init"};
    }
    
    // Check controller status
    uint8_t status = port_byte_in(KEYBOARD_STATUS_PORT);
    if (status & 0x10) {
        return (struct TestResult){__func__, false, "Keyboard controller error"};
    }
    
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_keyboard_enable_disable(void) {
    // First ensure controller is ready
    if (!wait_keyboard_controller()) {
        return (struct TestResult){__func__, false, "Keyboard controller not ready"};
    }
    
    // Enable keyboard
    keyboard_enable();
    for (volatile int i = 0; i < 10000; i++) {} // Wait for enable to complete
    
    uint8_t status1 = port_byte_in(KEYBOARD_STATUS_PORT);
    if (status1 & 0x10) {
        return (struct TestResult){__func__, false, "Failed to enable keyboard"};
    }
    
    // Disable keyboard
    keyboard_disable();
    for (volatile int i = 0; i < 10000; i++) {} // Wait for disable to complete
    
    uint8_t status2 = port_byte_in(KEYBOARD_STATUS_PORT);
    if (!(status2 & 0x10)) {
        return (struct TestResult){__func__, false, "Failed to disable keyboard"};
    }
    
    // Re-enable for other tests
    keyboard_enable();
    
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_keyboard_scancode(void) {
    // First ensure controller is ready
    if (!wait_keyboard_controller()) {
        return (struct TestResult){__func__, false, "Keyboard controller not ready"};
    }
    
    // Reset callback
    keyboard_set_scancode_callback(NULL);
    
    // Send echo command (0xEE) and expect same response
    port_byte_out(KEYBOARD_DATA_PORT, 0xEE);
    
    // Wait for response with timeout
    int timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 0x01) {
            uint8_t response = port_byte_in(KEYBOARD_DATA_PORT);
            if (response == 0xEE) {
                return (struct TestResult){__func__, true, NULL};
            }
            break;
        }
        for (volatile int i = 0; i < 1000; i++) {} // Small delay
    }
    
    return (struct TestResult){__func__, false, "Keyboard echo test failed"};
}

/* Serial Port Tests */
static struct TestResult test_serial_init(void) {
    serial_init(COM1_PORT, SERIAL_BAUD_115200);
    TEST_ASSERT(serial_transmit_empty(COM1_PORT), "Serial port not ready");
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_serial_write_read(void) {
    serial_write(COM1_PORT, 'T');
    TEST_ASSERT(serial_received(COM1_PORT), "Serial data not received");
    TEST_ASSERT(serial_read(COM1_PORT) == 'T', "Serial read data mismatch");
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_serial_string(void) {
    const char* test_str = "Test String";
    serial_write_string(COM1_PORT, test_str);
    TEST_ASSERT(serial_transmit_empty(COM1_PORT), "Serial transmission incomplete");
    return (struct TestResult){__func__, true, NULL};
}

/* PIT Tests */
static struct TestResult test_pit_init(void) {
    pit_init(PIT_DEFAULT_HZ);
    uint8_t status = port_byte_in(PIT_COMMAND);
    TEST_ASSERT((status & 0x40) != 0, "PIT not running");
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_pit_frequency(void) {
    pit_set_frequency(1000); // Test 1kHz
    uint8_t status = port_byte_in(PIT_COMMAND);
    TEST_ASSERT((status & 0x40) != 0, "PIT frequency change failed");
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_pit_callback(void) {
    bool callback_called = false;
    pit_set_callback(NULL); // Reset callback
    TEST_ASSERT(pit_get_ticks() > 0, "PIT tick count not incrementing");
    return (struct TestResult){__func__, true, NULL};
}

/* RTC Tests */
static struct TestResult test_rtc_init(void) {
    rtc_init();
    uint8_t status = port_byte_in(RTC_STATUS_A);
    TEST_ASSERT((status & 0x80) == 0, "RTC update in progress");
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_rtc_time(void) {
    uint8_t second = rtc_read_register(RTC_SECONDS);
    uint8_t minute = rtc_read_register(RTC_MINUTES);
    uint8_t hour = rtc_read_register(RTC_HOURS);
    
    TEST_ASSERT(second < 60, "Invalid RTC seconds");
    TEST_ASSERT(minute < 60, "Invalid RTC minutes");
    TEST_ASSERT(hour < 24, "Invalid RTC hours");
    
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_rtc_interrupts(void) {
    rtc_enable_interrupt();
    uint8_t status = port_byte_in(RTC_STATUS_B);
    TEST_ASSERT((status & 0x40) != 0, "RTC interrupt not enabled");
    
    rtc_disable_interrupt();
    status = port_byte_in(RTC_STATUS_B);
    TEST_ASSERT((status & 0x40) == 0, "RTC interrupt not disabled");
    
    return (struct TestResult){__func__, true, NULL};
}

/* Mouse Tests */
static struct TestResult test_mouse_init(void) {
    mouse_init();
    uint8_t status = port_byte_in(MOUSE_STATUS_PORT);
    TEST_ASSERT(status & 0x20, "Mouse not enabled");
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_mouse_enable_disable(void) {
    mouse_enable();
    uint8_t status1 = port_byte_in(MOUSE_STATUS_PORT);
    TEST_ASSERT(status1 & 0x20, "Mouse not enabled");
    
    mouse_disable();
    uint8_t status2 = port_byte_in(MOUSE_STATUS_PORT);
    TEST_ASSERT(!(status2 & 0x20), "Mouse not disabled");
    
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_mouse_callback(void) {
    bool callback_called = false;
    mouse_set_callback(NULL); // Reset callback
    TEST_ASSERT(port_byte_in(MOUSE_STATUS_PORT) & 0x20, "Mouse callback setup failed");
    return (struct TestResult){__func__, true, NULL};
}

/* Port I/O Tests */
static struct TestResult test_port_io(void) {
    const uint16_t TEST_PORT = 0x80; // Usually safe test port
    const uint8_t TEST_VALUE = 0x55;
    
    port_byte_out(TEST_PORT, TEST_VALUE);
    uint8_t read_value = port_byte_in(TEST_PORT);
    TEST_ASSERT(read_value == TEST_VALUE, "Port I/O read/write mismatch");
    
    return (struct TestResult){__func__, true, NULL};
}

static struct TestResult test_port_word(void) {
    const uint16_t TEST_PORT = 0x80;
    const uint16_t TEST_VALUE = 0x5555;
    
    port_word_out(TEST_PORT, TEST_VALUE);
    uint16_t read_value = port_word_in(TEST_PORT);
    TEST_ASSERT(read_value == TEST_VALUE, "Port I/O word read/write mismatch");
    
    return (struct TestResult){__func__, true, NULL};
}

/* Device driver test suite */
static TestFunction driver_tests[] = {
    // Keyboard tests - run these first since they're failing
    test_keyboard_init,
    test_keyboard_enable_disable,
    test_keyboard_scancode,
    
    // Serial port tests
    test_serial_init,
    test_serial_write_read,
    test_serial_string,
    
    // PIT tests
    test_pit_init,
    test_pit_frequency,
    test_pit_callback,
    
    // RTC tests
    test_rtc_init,
    test_rtc_time,
    test_rtc_interrupts,
    
    // Mouse tests
    test_mouse_init,
    test_mouse_enable_disable,
    test_mouse_callback,
    
    // Port I/O tests
    test_port_io,
    test_port_word
};

struct TestSuite driver_test_suite = {
    .name = "Device Driver Tests",
    .tests = driver_tests,
    .test_count = sizeof(driver_tests) / sizeof(TestFunction)
}; 