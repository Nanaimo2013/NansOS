/**
 * PS/2 Mouse Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>
#include "../../../impl/kernel/asm_utils.h"

/* Mouse ports (same as keyboard) */
#define MOUSE_DATA_PORT       0x60
#define MOUSE_STATUS_PORT     0x64
#define MOUSE_COMMAND_PORT    0x64

/* Mouse commands */
#define MOUSE_CMD_RESET       0xFF
#define MOUSE_CMD_ENABLE      0xF4
#define MOUSE_CMD_DISABLE     0xF5
#define MOUSE_CMD_SET_DEFAULTS 0xF6

/* Mouse button states */
#define MOUSE_LEFT_BUTTON    0x01
#define MOUSE_RIGHT_BUTTON   0x02
#define MOUSE_MIDDLE_BUTTON  0x04

/* Mouse packet structure */
struct MousePacket {
    uint8_t buttons;
    int16_t x_movement;
    int16_t y_movement;
    int8_t z_movement;    /* Scroll wheel */
};

/* Mouse state */
struct MouseState {
    uint16_t x_pos;
    uint16_t y_pos;
    uint8_t buttons;
    int8_t scroll;
};

/* Global mouse state */
extern struct MouseState mouse_state;

/* Function declarations */
void mouse_init(void);              /* Initialize mouse */
void mouse_handler(void);           /* Mouse interrupt handler */
void mouse_enable(void);            /* Enable mouse */
void mouse_disable(void);           /* Disable mouse */

/* Callback type for mouse events */
typedef void (*mouse_callback_t)(struct MouseState* state);

/* Set callback for mouse events */
void mouse_set_callback(mouse_callback_t callback); 