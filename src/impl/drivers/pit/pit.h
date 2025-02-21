/**
 * Programmable Interval Timer (PIT) Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* PIT ports */
#define PIT_CHANNEL0    0x40    /* Channel 0 data port */
#define PIT_CHANNEL1    0x41    /* Channel 1 data port */
#define PIT_CHANNEL2    0x42    /* Channel 2 data port */
#define PIT_COMMAND     0x43    /* Mode/Command register */

/* PIT commands */
#define PIT_CHANNEL0_SELECT     0x00    /* Select channel 0 */
#define PIT_LOBYTE_HIBYTE      0x30    /* Access mode: low byte/high byte */
#define PIT_SQUARE_WAVE        0x06    /* Square wave generator */
#define PIT_BINARY_MODE        0x00    /* 16-bit binary mode */

/* PIT frequency */
#define PIT_FREQUENCY    1193180        /* Base frequency: 1.193180 MHz */
#define PIT_DEFAULT_HZ   100            /* Default timer frequency: 100 Hz */

/* Function declarations */
void pit_init(uint32_t frequency);      /* Initialize PIT with given frequency */
void pit_set_frequency(uint32_t hz);    /* Set PIT frequency in Hz */
uint64_t pit_get_ticks(void);          /* Get number of ticks since boot */
void pit_handler(void);                 /* Timer interrupt handler */

/* Timer callback type */
typedef void (*timer_callback_t)(void);

/* Set callback for timer events */
void pit_set_callback(timer_callback_t callback); 