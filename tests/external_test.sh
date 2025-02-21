#!/bin/bash

# External Test Framework for NansOS
# This script runs tests from outside the OS using QEMU's debugging features

# Colors for output (Windows-safe)
RED=""
GREEN=""
YELLOW=""
NC=""

# Set up test directory and log file
TEST_DIR="test_output"
TEST_LOG="$TEST_DIR/serial.log"

# Test if Docker is installed
if ! command -v docker &> /dev/null; then
    echo -e "${RED}Error: Docker is not installed${NC}"
    exit 1
fi

# Test if QEMU is installed
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo -e "${RED}Error: QEMU is not installed${NC}"
    exit 1
fi

# Create test directory
rm -rf "$TEST_DIR" 2>/dev/null || true
mkdir -p "$TEST_DIR"

# Function to kill QEMU process
kill_qemu() {
    local pid=$1
    if [ "$OS" = "Windows_NT" ]; then
        taskkill //F //PID $pid //T 2>/dev/null || true
    else
        kill -9 $pid 2>/dev/null || true
    fi
    sleep 1
}

# Function to run a test
run_test() {
    local test_name=$1
    local timeout=$2
    local success_pattern=$3
    
    echo "Running test: ${test_name}"
    
    # Clear the log file
    echo "" > "$TEST_LOG"
    
    # Run QEMU with serial output to file
    qemu-system-x86_64 \
        -cdrom dist/x86_64/kernel.iso \
        -nographic \
        -serial file:$TEST_LOG \
        -display none \
        -no-reboot \
        -monitor none \
        -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
        > /dev/null 2>&1 &
    
    QEMU_PID=$!
    
    # Wait for timeout or success pattern
    local elapsed=0
    while [ $elapsed -lt $timeout ]; do
        if grep -q "$success_pattern" "$TEST_LOG" 2>/dev/null; then
            echo "✓ Test passed: ${test_name}"
            kill_qemu $QEMU_PID
            return 0
        fi
        sleep 1
        ((elapsed++))
    done
    
    # Print the log file for debugging
    echo "Test log:"
    cat "$TEST_LOG" 2>/dev/null || true
    
    echo "✗ Test failed: ${test_name} (timeout)"
    kill_qemu $QEMU_PID
    return 1
}

# Build the kernel
echo "Building NansOS..."
if [ "$OS" = "Windows_NT" ]; then
    WORKSPACE_DIR=$(pwd -W)
    docker run --rm -v "${WORKSPACE_DIR}:/root/env" nansos-buildenv make build-x86_64
else
    docker run --rm -v "${PWD}:/root/env" nansos-buildenv make build-x86_64
fi

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo "Build failed! Cannot continue with tests."
    exit 1
fi

# Initialize test results
TOTAL_TESTS=0
PASSED_TESTS=0

# Test 1: Boot Success
((TOTAL_TESTS++))
if run_test "Boot Success" 10 "NansOS initialized and ready!"; then
    ((PASSED_TESTS++))
fi

# Test 2: Memory Management
((TOTAL_TESTS++))
if run_test "Memory Management" 10 "Memory management initialized"; then
    ((PASSED_TESTS++))
fi

# Test 3: Interrupt Setup
((TOTAL_TESTS++))
if run_test "Interrupt Setup" 10 "Interrupts enabled"; then
    ((PASSED_TESTS++))
fi

# Test 4: Device Initialization
((TOTAL_TESTS++))
if run_test "Device Initialization" 10 "System initialization complete!"; then
    ((PASSED_TESTS++))
fi

# Test 5: Keyboard Controller Tests
((TOTAL_TESTS++))
if run_test "Keyboard Echo Test" 10 "[KBD] Echo test passed"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "Keyboard LED Test" 10 "[KBD] LED control test passed"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "Keyboard Enable" 10 "[KBD] Keyboard enabled"; then
    ((PASSED_TESTS++))
fi

# Test 6: Mouse Tests
((TOTAL_TESTS++))
if run_test "Mouse Init" 10 "[KBD] PS/2 mouse initialized"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "Mouse Ready" 10 "Mouse initialization complete"; then
    ((PASSED_TESTS++))
fi

# Test 7: Timer Tests
((TOTAL_TESTS++))
if run_test "PIT Init" 10 "[PIT] Timer initialized"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "Timer Tick" 15 "Uptime: 1 seconds"; then
    ((PASSED_TESTS++))
fi

# Test 8: RTC Tests
((TOTAL_TESTS++))
if run_test "RTC Init" 10 "[DEBUG] RTC initialized"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "RTC Interrupt" 10 "RTC interrupt"; then
    ((PASSED_TESTS++))
fi

# Test 9: Serial Port Tests
((TOTAL_TESTS++))
if run_test "Serial Init" 10 "Serial port initialized"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "Serial Debug" 10 "[DEBUG]"; then
    ((PASSED_TESTS++))
fi

# Test 10: System Information Tests
((TOTAL_TESTS++))
if run_test "CPU Info" 15 "CPU Features:"; then
    ((PASSED_TESTS++))
fi

((TOTAL_TESTS++))
if run_test "Memory Info" 15 "Memory information updated"; then
    ((PASSED_TESTS++))
fi

# Test 11: Error Handling Tests
((TOTAL_TESTS++))
if run_test "Error Handler" 10 "FATAL ERROR:"; then
    ((PASSED_TESTS++))
fi

# Test 12: Full System Integration
((TOTAL_TESTS++))
if run_test "System Ready" 20 "Entering main kernel loop"; then
    ((PASSED_TESTS++))
fi

# Test 13: Keyboard Input Tests
((TOTAL_TESTS++))
if run_test "Keyboard Input" 15 "Scancode: 0x"; then
    ((PASSED_TESTS++))
fi

# Test 14: Mouse Movement Tests
((TOTAL_TESTS++))
if run_test "Mouse Movement" 15 "Mouse: X="; then
    ((PASSED_TESTS++))
fi

# Test 15: Special Key Tests
((TOTAL_TESTS++))
if run_test "Special Keys" 15 "Special Key:"; then
    ((PASSED_TESTS++))
fi

# Print test summary
echo
echo "Test Summary"
echo "============"
echo "Total Tests: ${TOTAL_TESTS}"
echo "Passed: ${PASSED_TESTS}"
echo "Failed: $((TOTAL_TESTS - PASSED_TESTS))"
echo "Pass Rate: $(( (PASSED_TESTS * 100) / TOTAL_TESTS ))%"

# Print test categories summary
echo
echo "Test Categories"
echo "==============="
echo "Boot & Initialization: Tests 1-4"
echo "Keyboard & Mouse: Tests 5-6"
echo "Timer & RTC: Tests 7-8"
echo "Serial & Debug: Test 9"
echo "System Info: Test 10"
echo "Error Handling: Test 11"
echo "System Integration: Test 12"
echo "Input Handling: Tests 13-15"

# Exit with status based on test results
[ $PASSED_TESTS -eq $TOTAL_TESTS ] 