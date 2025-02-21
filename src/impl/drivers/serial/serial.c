/**
 * Serial Port Driver Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include <stddef.h>
#include <stdint.h>
#include "serial.h"
#include "../port_io/port.h"

void serial_init(uint16_t port, uint16_t baud) {
    /* Disable interrupts */
    port_byte_out(port + SERIAL_INT_ENABLE, 0x00);

    /* Set DLAB to access baud rate divisor */
    port_byte_out(port + SERIAL_LINE_CTRL, SERIAL_LC_DLAB);

    /* Set baud rate divisor */
    port_byte_out(port + SERIAL_DIVISOR_LOW, baud & 0xFF);
    port_byte_out(port + SERIAL_DIVISOR_HIGH, (baud >> 8) & 0xFF);

    /* 8 bits, no parity, one stop bit */
    port_byte_out(port + SERIAL_LINE_CTRL, SERIAL_LC_8BITS);

    /* Enable FIFO, clear them, with 14-byte threshold */
    port_byte_out(port + SERIAL_INT_ID, 0xC7);

    /* IRQs enabled, RTS/DSR set */
    port_byte_out(port + SERIAL_MODEM_CTRL, 0x0B);
}

int serial_received(uint16_t port) {
    return port_byte_in(port + SERIAL_LINE_STATUS) & 1;
}

char serial_read(uint16_t port) {
    while (serial_received(port) == 0);
    return port_byte_in(port);
}

int serial_transmit_empty(uint16_t port) {
    return port_byte_in(port + SERIAL_LINE_STATUS) & 0x20;
}

void serial_write(uint16_t port, char c) {
    while (serial_transmit_empty(port) == 0);
    port_byte_out(port, c);
}

void serial_write_string(uint16_t port, const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        serial_write(port, str[i]);
    }
} 