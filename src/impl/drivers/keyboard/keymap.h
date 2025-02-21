/**
 * Keyboard Scancode Mappings
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* Key states */
#define KEY_RELEASED     0x80
#define KEY_PRESSED      0x00

/* Special keys */
#define KEY_ESCAPE       0x01
#define KEY_BACKSPACE    0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LCTRL       0x1D
#define KEY_RCTRL       0x1D    /* With E0 prefix */
#define KEY_LSHIFT      0x2A
#define KEY_RSHIFT      0x36
#define KEY_LALT        0x38
#define KEY_RALT        0x38    /* With E0 prefix */
#define KEY_CAPSLOCK    0x3A
#define KEY_F1          0x3B
#define KEY_F2          0x3C
#define KEY_F3          0x3D
#define KEY_F4          0x3E
#define KEY_F5          0x3F
#define KEY_F6          0x40
#define KEY_F7          0x41
#define KEY_F8          0x42
#define KEY_F9          0x43
#define KEY_F10         0x44
#define KEY_F11         0x57
#define KEY_F12         0x58
#define KEY_NUMLOCK     0x45
#define KEY_SCROLLLOCK  0x46
#define KEY_HOME        0x47
#define KEY_UP          0x48
#define KEY_PAGEUP      0x49
#define KEY_LEFT        0x4B
#define KEY_RIGHT       0x4D
#define KEY_END         0x4F
#define KEY_DOWN        0x50
#define KEY_PAGEDOWN    0x51
#define KEY_INSERT      0x52
#define KEY_DELETE      0x53

/* Extended key prefix */
#define KEY_EXTENDED    0xE0

/* Special key combinations */
#define CTRL_A          0x01    /* Ctrl + A */
#define CTRL_B          0x02    /* Ctrl + B */
#define CTRL_C          0x03    /* Ctrl + C */
#define CTRL_D          0x04    /* Ctrl + D */
#define CTRL_E          0x05    /* Ctrl + E */
#define CTRL_F          0x06    /* Ctrl + F */
#define CTRL_G          0x07    /* Ctrl + G */
#define CTRL_H          0x08    /* Ctrl + H */
#define CTRL_I          0x09    /* Ctrl + I */
#define CTRL_J          0x0A    /* Ctrl + J */
#define CTRL_K          0x0B    /* Ctrl + K */
#define CTRL_L          0x0C    /* Ctrl + L */
#define CTRL_M          0x0D    /* Ctrl + M */
#define CTRL_N          0x0E    /* Ctrl + N */
#define CTRL_O          0x0F    /* Ctrl + O */
#define CTRL_P          0x10    /* Ctrl + P */
#define CTRL_Q          0x11    /* Ctrl + Q */
#define CTRL_R          0x12    /* Ctrl + R */
#define CTRL_S          0x13    /* Ctrl + S */
#define CTRL_T          0x14    /* Ctrl + T */
#define CTRL_U          0x15    /* Ctrl + U */
#define CTRL_V          0x16    /* Ctrl + V */
#define CTRL_W          0x17    /* Ctrl + W */
#define CTRL_X          0x18    /* Ctrl + X */
#define CTRL_Y          0x19    /* Ctrl + Y */
#define CTRL_Z          0x1A    /* Ctrl + Z */

/* Keyboard state */
struct KeyboardState {
    uint8_t shift_pressed : 1;
    uint8_t ctrl_pressed : 1;
    uint8_t alt_pressed : 1;
    uint8_t caps_lock : 1;
    uint8_t num_lock : 1;
    uint8_t scroll_lock : 1;
    uint8_t extended : 1;    /* For extended scancodes */
    uint8_t reserved : 1;
};

/* Special key event */
struct SpecialKeyEvent {
    uint8_t key_code;        /* The key code (F1-F12, arrows, etc.) */
    uint8_t is_pressed;      /* Whether the key was pressed or released */
    struct KeyboardState state;  /* State of modifier keys */
};

/* Function declarations */
char scancode_to_ascii(uint8_t scancode, struct KeyboardState* state);
void update_keyboard_state(uint8_t scancode, struct KeyboardState* state);

/* Special key handling */
int is_special_key(uint8_t scancode);
void handle_special_key(uint8_t scancode, struct KeyboardState* state, struct SpecialKeyEvent* event);

/* Callback types */
typedef void (*special_key_callback_t)(struct SpecialKeyEvent* event);

/* Set callback for special keys */
void keyboard_set_special_callback(special_key_callback_t callback); 