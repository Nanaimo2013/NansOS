/**
 * Programmable Interval Timer (PIT) Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "pit.h"
#include "../port_io/port.h"
#include "../pic/pic.h"
#include "../serial/serial.h"

/* PIT IRQ number */
#define PIT_IRQ 0

/* Internal variables */
static uint64_t tick_count = 0;
static timer_callback_t timer_callback = 0;

void pit_init(uint32_t frequency) {
    /* Set up the timer frequency */
    pit_set_frequency(frequency);
    
    /* Enable timer interrupt */
    pic_clear_mask(PIT_IRQ);
    
    /* Send initialization message */
    serial_write_string(COM1_PORT, "[PIT] Timer initialized\n");
}

void pit_set_frequency(uint32_t hz) {
    /* Calculate divisor */
    uint32_t divisor = PIT_FREQUENCY / hz;
    
    /* Send command byte */
    port_byte_out(PIT_COMMAND, PIT_CHANNEL0_SELECT | PIT_LOBYTE_HIBYTE | PIT_SQUARE_WAVE | PIT_BINARY_MODE);
    
    /* Set frequency divisor */
    port_byte_out(PIT_CHANNEL0, divisor & 0xFF);           /* Low byte */
    port_byte_out(PIT_CHANNEL0, (divisor >> 8) & 0xFF);    /* High byte */
}

void pit_handler(void) {
    tick_count++;
    
    /* Send heartbeat every second */
    if (tick_count % PIT_DEFAULT_HZ == 0) {
        serial_write_string(COM1_PORT, ".");
    }
    
    /* Call timer callback if registered */
    if (timer_callback) {
        timer_callback();
    }
    
    /* Send EOI to PIC */
    pic_send_eoi(PIT_IRQ);
}

uint64_t pit_get_ticks(void) {
    return tick_count;
}

void pit_set_callback(timer_callback_t callback) {
    timer_callback = callback;
} 