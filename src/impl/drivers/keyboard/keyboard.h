/**
 * PS/2 Keyboard Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>
#include "keymap.h"

/* Keyboard ports */
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64
#define KEYBOARD_COMMAND_PORT   0x64

/* Keyboard commands */
#define KEYBOARD_CMD_LED       0xED
#define KEYBOARD_CMD_ECHO      0xEE
#define KEYBOARD_CMD_SCANCODE  0xF0
#define KEYBOARD_CMD_IDENTIFY  0xF2
#define KEYBOARD_CMD_RATE      0xF3
#define KEYBOARD_CMD_ENABLE    0xF4
#define KEYBOARD_CMD_DISABLE   0xF5
#define KEYBOARD_CMD_RESET     0xFF

/* Function declarations */
void keyboard_init(void);              /* Initialize keyboard */
void keyboard_handler(void);           /* Keyboard interrupt handler */
uint8_t keyboard_read_scan_code(void); /* Read scan code from keyboard */
void keyboard_enable(void);            /* Enable keyboard */
void keyboard_disable(void);           /* Disable keyboard */

/* Callback types */
typedef void (*keyboard_scancode_callback_t)(uint8_t scancode);
typedef void (*keyboard_char_callback_t)(char c);

/* Set callbacks */
void keyboard_set_scancode_callback(keyboard_scancode_callback_t callback);
void keyboard_set_char_callback(keyboard_char_callback_t callback); 