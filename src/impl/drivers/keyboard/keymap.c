/**
 * Keyboard Scancode Mappings Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "keymap.h"

/* Internal callback for special keys */
special_key_callback_t special_callback = 0;

/* US QWERTY keyboard layout */
static const char ascii_lowercase[] = {
    0,    /* 0x00 */
    0,    /* 0x01 - ESC */
    '1',  /* 0x02 */
    '2',  /* 0x03 */
    '3',  /* 0x04 */
    '4',  /* 0x05 */
    '5',  /* 0x06 */
    '6',  /* 0x07 */
    '7',  /* 0x08 */
    '8',  /* 0x09 */
    '9',  /* 0x0A */
    '0',  /* 0x0B */
    '-',  /* 0x0C */
    '=',  /* 0x0D */
    '\b', /* 0x0E - Backspace */
    '\t', /* 0x0F - Tab */
    'q',  /* 0x10 */
    'w',  /* 0x11 */
    'e',  /* 0x12 */
    'r',  /* 0x13 */
    't',  /* 0x14 */
    'y',  /* 0x15 */
    'u',  /* 0x16 */
    'i',  /* 0x17 */
    'o',  /* 0x18 */
    'p',  /* 0x19 */
    '[',  /* 0x1A */
    ']',  /* 0x1B */
    '\n', /* 0x1C - Enter */
    0,    /* 0x1D - Left Control */
    'a',  /* 0x1E */
    's',  /* 0x1F */
    'd',  /* 0x20 */
    'f',  /* 0x21 */
    'g',  /* 0x22 */
    'h',  /* 0x23 */
    'j',  /* 0x24 */
    'k',  /* 0x25 */
    'l',  /* 0x26 */
    ';',  /* 0x27 */
    '\'', /* 0x28 */
    '`',  /* 0x29 */
    0,    /* 0x2A - Left Shift */
    '\\', /* 0x2B */
    'z',  /* 0x2C */
    'x',  /* 0x2D */
    'c',  /* 0x2E */
    'v',  /* 0x2F */
    'b',  /* 0x30 */
    'n',  /* 0x31 */
    'm',  /* 0x32 */
    ',',  /* 0x33 */
    '.',  /* 0x34 */
    '/',  /* 0x35 */
    0,    /* 0x36 - Right Shift */
    '*',  /* 0x37 */
    0,    /* 0x38 - Left Alt */
    ' ',  /* 0x39 - Space */
};

static const char ascii_uppercase[] = {
    0,    /* 0x00 */
    0,    /* 0x01 - ESC */
    '!',  /* 0x02 */
    '@',  /* 0x03 */
    '#',  /* 0x04 */
    '$',  /* 0x05 */
    '%',  /* 0x06 */
    '^',  /* 0x07 */
    '&',  /* 0x08 */
    '*',  /* 0x09 */
    '(',  /* 0x0A */
    ')',  /* 0x0B */
    '_',  /* 0x0C */
    '+',  /* 0x0D */
    '\b', /* 0x0E - Backspace */
    '\t', /* 0x0F - Tab */
    'Q',  /* 0x10 */
    'W',  /* 0x11 */
    'E',  /* 0x12 */
    'R',  /* 0x13 */
    'T',  /* 0x14 */
    'Y',  /* 0x15 */
    'U',  /* 0x16 */
    'I',  /* 0x17 */
    'O',  /* 0x18 */
    'P',  /* 0x19 */
    '{',  /* 0x1A */
    '}',  /* 0x1B */
    '\n', /* 0x1C - Enter */
    0,    /* 0x1D - Left Control */
    'A',  /* 0x1E */
    'S',  /* 0x1F */
    'D',  /* 0x20 */
    'F',  /* 0x21 */
    'G',  /* 0x22 */
    'H',  /* 0x23 */
    'J',  /* 0x24 */
    'K',  /* 0x25 */
    'L',  /* 0x26 */
    ':',  /* 0x27 */
    '"',  /* 0x28 */
    '~',  /* 0x29 */
    0,    /* 0x2A - Left Shift */
    '|',  /* 0x2B */
    'Z',  /* 0x2C */
    'X',  /* 0x2D */
    'C',  /* 0x2E */
    'V',  /* 0x2F */
    'B',  /* 0x30 */
    'N',  /* 0x31 */
    'M',  /* 0x32 */
    '<',  /* 0x33 */
    '>',  /* 0x34 */
    '?',  /* 0x35 */
    0,    /* 0x36 - Right Shift */
    '*',  /* 0x37 */
    0,    /* 0x38 - Left Alt */
    ' ',  /* 0x39 - Space */
};

int is_special_key(uint8_t scancode) {
    uint8_t key = scancode & ~KEY_RELEASED;
    return (key >= KEY_F1 && key <= KEY_F12) ||
           (key >= KEY_HOME && key <= KEY_DELETE) ||
           key == KEY_ESCAPE;
}

void handle_special_key(uint8_t scancode, struct KeyboardState* state, struct SpecialKeyEvent* event) {
    event->key_code = scancode & ~KEY_RELEASED;
    event->is_pressed = !(scancode & KEY_RELEASED);
    event->state = *state;

    if (special_callback) {
        special_callback(event);
    }
}

void update_keyboard_state(uint8_t scancode, struct KeyboardState* state) {
    /* Handle extended key prefix */
    if (scancode == KEY_EXTENDED) {
        state->extended = 1;
        return;
    }

    uint8_t key = scancode & ~KEY_RELEASED;
    uint8_t released = scancode & KEY_RELEASED;

    /* Handle the key based on whether it's extended or not */
    if (state->extended) {
        switch(key) {
            case KEY_RCTRL:
                state->ctrl_pressed = !released;
                break;
            case KEY_RALT:
                state->alt_pressed = !released;
                break;
        }
        state->extended = 0;
    } else {
        switch(key) {
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                state->shift_pressed = !released;
                break;
            case KEY_LCTRL:
                state->ctrl_pressed = !released;
                break;
            case KEY_LALT:
                state->alt_pressed = !released;
                break;
            case KEY_CAPSLOCK:
                if (!released) {
                    state->caps_lock = !state->caps_lock;
                }
                break;
            case KEY_NUMLOCK:
                if (!released) {
                    state->num_lock = !state->num_lock;
                }
                break;
            case KEY_SCROLLLOCK:
                if (!released) {
                    state->scroll_lock = !state->scroll_lock;
                }
                break;
        }
    }
}

char scancode_to_ascii(uint8_t scancode, struct KeyboardState* state) {
    uint8_t key = scancode & ~KEY_RELEASED;
    
    /* Ignore key releases and extended keys */
    if ((scancode & KEY_RELEASED) || state->extended) {
        return 0;
    }
    
    /* Check if key is in range */
    if (key >= sizeof(ascii_lowercase)) {
        return 0;
    }
    
    /* Handle Ctrl combinations */
    if (state->ctrl_pressed && key >= 0x1E && key <= 0x39) {
        char base = ascii_lowercase[key];
        if (base >= 'a' && base <= 'z') {
            return base - 'a' + 1;  /* Convert to Ctrl codes (1-26) */
        }
    }
    
    /* Determine if we should use uppercase */
    int use_uppercase = (state->shift_pressed || state->caps_lock) && 
                       !(state->shift_pressed && state->caps_lock);
                       
    /* Return appropriate character */
    return use_uppercase ? ascii_uppercase[key] : ascii_lowercase[key];
} 