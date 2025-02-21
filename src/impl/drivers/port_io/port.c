/**
 * Port I/O Operations Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "port.h"
#include "../../kernel/asm_utils.h"

void port_byte_out(uint16_t port, uint8_t value) {
    asm_outb(port, value);
}

uint8_t port_byte_in(uint16_t port) {
    return asm_inb(port);
}

void port_word_out(uint16_t port, uint16_t value) {
    /* Split word into bytes */
    port_byte_out(port, (uint8_t)value);
    port_byte_out(port + 1, (uint8_t)(value >> 8));
}

uint16_t port_word_in(uint16_t port) {
    /* Combine bytes into word */
    uint16_t result = port_byte_in(port);
    result |= ((uint16_t)port_byte_in(port + 1) << 8);
    return result;
}

void port_dword_out(uint16_t port, uint32_t value) {
    /* Split dword into bytes */
    port_byte_out(port, (uint8_t)value);
    port_byte_out(port + 1, (uint8_t)(value >> 8));
    port_byte_out(port + 2, (uint8_t)(value >> 16));
    port_byte_out(port + 3, (uint8_t)(value >> 24));
}

uint32_t port_dword_in(uint16_t port) {
    /* Combine bytes into dword */
    uint32_t result = port_byte_in(port);
    result |= ((uint32_t)port_byte_in(port + 1) << 8);
    result |= ((uint32_t)port_byte_in(port + 2) << 16);
    result |= ((uint32_t)port_byte_in(port + 3) << 24);
    return result;
}

void port_io_wait(void) {
    /* Write to unused port 0x80 to add a small delay */
    port_byte_out(0x80, 0);
} 