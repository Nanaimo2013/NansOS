/**
 * Serial Port Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* COM port addresses */
#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8
#define COM3_PORT 0x3E8
#define COM4_PORT 0x2E8

/* Port offsets */
#define SERIAL_DATA         0x00    /* Data register (read/write) */
#define SERIAL_INT_ENABLE   0x01    /* Interrupt enable */
#define SERIAL_DIVISOR_LOW  0x00    /* Divisor latch low byte */
#define SERIAL_DIVISOR_HIGH 0x01    /* Divisor latch high byte */
#define SERIAL_INT_ID      0x02    /* Interrupt identification and FIFO control */
#define SERIAL_LINE_CTRL   0x03    /* Line control register */
#define SERIAL_MODEM_CTRL  0x04    /* Modem control register */
#define SERIAL_LINE_STATUS 0x05    /* Line status register */
#define SERIAL_MODEM_STATUS 0x06   /* Modem status register */

/* Line control bits */
#define SERIAL_LC_5BITS    0x00    /* 5 data bits */
#define SERIAL_LC_6BITS    0x01    /* 6 data bits */
#define SERIAL_LC_7BITS    0x02    /* 7 data bits */
#define SERIAL_LC_8BITS    0x03    /* 8 data bits */
#define SERIAL_LC_1STOP    0x00    /* 1 stop bit */
#define SERIAL_LC_2STOP    0x04    /* 2 stop bits */
#define SERIAL_LC_PARITY   0x08    /* Enable parity */
#define SERIAL_LC_DLAB     0x80    /* Divisor latch access */

/* Common baud rates */
#define SERIAL_BAUD_115200 1
#define SERIAL_BAUD_57600  2
#define SERIAL_BAUD_38400  3
#define SERIAL_BAUD_19200  6
#define SERIAL_BAUD_9600   12
#define SERIAL_BAUD_4800   24
#define SERIAL_BAUD_2400   48

/* Function declarations */
void serial_init(uint16_t port, uint16_t baud);  /* Initialize serial port */
void serial_write(uint16_t port, char c);        /* Write character to port */
char serial_read(uint16_t port);                 /* Read character from port */
int serial_received(uint16_t port);              /* Check if data available */
int serial_transmit_empty(uint16_t port);        /* Check if transmit buffer empty */

/* Write string to serial port */
void serial_write_string(uint16_t port, const char* str); 