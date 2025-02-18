#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Function to print status messages
print_status() {
    echo -e "${GREEN}[*]${NC} $1"
}

print_error() {
    echo -e "${RED}[!]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Check for required tools
check_dependencies() {
    print_status "Checking dependencies..."
    
    local missing=0
    for cmd in nasm ld qemu-system-x86_64; do
        if ! command -v $cmd &> /dev/null; then
            print_error "Missing dependency: $cmd"
            missing=1
        fi
    done
    
    if [ $missing -eq 1 ]; then
        print_error "Please install missing dependencies and try again"
        exit 1
    fi
    
    print_status "All dependencies found"
}

# Clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    make clean
}

# Build the OS
build_os() {
    print_status "Building NansOS..."
    if make all; then
        print_status "Build successful"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Run the OS
run_os() {
    print_status "Running NansOS in QEMU..."
    make run
}

# Run the OS with debugging enabled
debug_os() {
    print_status "Running NansOS in debug mode..."
    print_warning "Connect GDB to localhost:1234 to begin debugging"
    make debug
}

# Main script
main() {
    # Process command line arguments
    case "$1" in
        "clean")
            clean_build
            ;;
        "build")
            check_dependencies
            clean_build
            build_os
            ;;
        "debug")
            check_dependencies
            clean_build
            build_os
            debug_os
            ;;
        *)
            check_dependencies
            clean_build
            build_os
            run_os
            ;;
    esac
}

# Run main function with all arguments
main "$@" 