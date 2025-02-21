/**
 * PS/2 Keyboard Driver Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "keyboard.h"
#include "../port_io/port.h"
#include "../pic/pic.h"
#include "../serial/serial.h"
#include "../mouse/mouse.h"
#include "../../kernel/asm_utils.h"
#include <stdio.h>

/* Keyboard IRQ number */
#define KEYBOARD_IRQ 1

/* Internal callbacks */
static keyboard_scancode_callback_t scancode_callback = 0;
static keyboard_char_callback_t char_callback = 0;
static special_key_callback_t special_callback = 0;

/* Keyboard state */
static struct KeyboardState keyboard_state = {0};
static struct SpecialKeyEvent special_event = {0};

void keyboard_init(void) {
    serial_write_string(COM1_PORT, "[DEBUG] Starting keyboard initialization...\n");
    
    /* Disable interrupts during initialization */
    asm_cli();
    
    /* Wait for keyboard controller to be ready */
    int timeout = 100000;
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 2) != 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        serial_write_string(COM1_PORT, "[KBD] Error: Initial controller timeout\n");
        asm_sti();
        return;
    }
    
    /* Disable both keyboard and mouse first */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xAD);  /* Disable keyboard */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xA7);  /* Disable mouse */
    
    /* Flush the output buffer */
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 1) != 0) {
        port_byte_in(KEYBOARD_DATA_PORT);
    }
    
    /* Read current controller configuration */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0x20);
    timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            uint8_t config = port_byte_in(KEYBOARD_DATA_PORT);
            
            /* Enable keyboard interrupt and keyboard interface */
            config |= 1;       /* Enable IRQ */
            config &= ~(1 << 4); /* Enable keyboard interface */
            config &= ~(1 << 6); /* Enable translation */
            
            /* Write back configuration */
            port_byte_out(KEYBOARD_COMMAND_PORT, 0x60);
            port_byte_out(KEYBOARD_DATA_PORT, config);
            serial_write_string(COM1_PORT, "[DEBUG] Keyboard controller configured\n");
            break;
        }
    }
    if (timeout == 0) {
        serial_write_string(COM1_PORT, "[KBD] Error: Controller configuration failed\n");
        asm_sti();
        return;
    }
    
    /* Re-enable keyboard */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xAE);
    
    /* Reset keyboard */
    port_byte_out(KEYBOARD_DATA_PORT, 0xFF);
    timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            uint8_t response = port_byte_in(KEYBOARD_DATA_PORT);
            if (response == 0xFA) {
                serial_write_string(COM1_PORT, "[DEBUG] Keyboard reset acknowledged\n");
                break;
            }
        }
    }
    if (timeout == 0) {
        serial_write_string(COM1_PORT, "[KBD] Error: Keyboard reset failed\n");
        asm_sti();
        return;
    }
    
    /* Wait for self-test completion */
    timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            uint8_t response = port_byte_in(KEYBOARD_DATA_PORT);
            if (response == 0xAA) {
                serial_write_string(COM1_PORT, "[DEBUG] Keyboard self-test passed\n");
                break;
            }
        }
    }
    if (timeout == 0) {
        serial_write_string(COM1_PORT, "[KBD] Error: Keyboard self-test failed\n");
        asm_sti();
        return;
    }
    
    /* Enable keyboard */
    port_byte_out(KEYBOARD_DATA_PORT, 0xF4);
    timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            uint8_t response = port_byte_in(KEYBOARD_DATA_PORT);
            if (response == 0xFA) {
                serial_write_string(COM1_PORT, "[KBD] Keyboard enabled\n");
                break;
            }
        }
    }
    if (timeout == 0) {
        serial_write_string(COM1_PORT, "[KBD] Error: Keyboard enable failed\n");
        asm_sti();
        return;
    }
    
    /* Enable keyboard interrupt */
    pic_clear_mask(KEYBOARD_IRQ);
    serial_write_string(COM1_PORT, "[DEBUG] Keyboard interrupt enabled\n");
    
    /* Initialize mouse after keyboard is fully set up */
    serial_write_string(COM1_PORT, "[DEBUG] Starting mouse initialization...\n");
    mouse_init();
    
    /* Re-enable interrupts */
    asm_sti();
    
    serial_write_string(COM1_PORT, "[DEBUG] Keyboard initialization complete\n");
}

void keyboard_handler(void) {
    /* Disable interrupts during handler */
    asm_cli();
    
    /* Check if there's actually data to read */
    if (!(port_byte_in(KEYBOARD_STATUS_PORT) & 1)) {
        pic_send_eoi(KEYBOARD_IRQ);
        asm_sti();
        return;
    }
    
    uint8_t scancode = keyboard_read_scan_code();
    
    /* Send scancode to serial for debugging */
    char debug_msg[32];
    sprintf(debug_msg, "[KBD] Scancode: 0x%02x\n", scancode);
    serial_write_string(COM1_PORT, debug_msg);
    
    /* Update keyboard state */
    update_keyboard_state(scancode, &keyboard_state);
    
    /* Call scancode callback if registered */
    if (scancode_callback) {
        scancode_callback(scancode);
    }
    
    /* Handle special keys */
    if (is_special_key(scancode)) {
        handle_special_key(scancode, &keyboard_state, &special_event);
        if (special_callback) {
            special_callback(&special_event);
        }
    }
    /* Handle regular keys */
    else if (char_callback) {
        char c = scancode_to_ascii(scancode, &keyboard_state);
        if (c != 0) {
            char_callback(c);
        }
    }
    
    /* Send EOI to PIC */
    pic_send_eoi(KEYBOARD_IRQ);
    
    /* Re-enable interrupts */
    asm_sti();
}

uint8_t keyboard_read_scan_code(void) {
    /* Wait until data is available */
    int timeout = 10000;
    while (!(port_byte_in(KEYBOARD_STATUS_PORT) & 1) && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        return 0;  /* Return 0 on timeout */
    }
    return port_byte_in(KEYBOARD_DATA_PORT);
}

void keyboard_enable(void) {
    /* Wait for keyboard to be ready */
    int timeout = 10000;
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 2) != 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        return;
    }
    
    /* Send enable command */
    port_byte_out(KEYBOARD_DATA_PORT, KEYBOARD_CMD_ENABLE);
    
    /* Wait for acknowledgment with timeout */
    timeout = 10000;
    while (timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            if (port_byte_in(KEYBOARD_DATA_PORT) == 0xFA) {
                break;
            }
        }
        timeout--;
    }
}

void keyboard_disable(void) {
    /* Wait for keyboard to be ready */
    int timeout = 10000;
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 2) != 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        return;
    }
    
    /* Send disable command */
    port_byte_out(KEYBOARD_DATA_PORT, KEYBOARD_CMD_DISABLE);
    
    /* Wait for acknowledgment with timeout */
    timeout = 10000;
    while (timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            if (port_byte_in(KEYBOARD_DATA_PORT) == 0xFA) {
                break;
            }
        }
        timeout--;
    }
}

void keyboard_set_scancode_callback(keyboard_scancode_callback_t callback) {
    scancode_callback = callback;
}

void keyboard_set_char_callback(keyboard_char_callback_t callback) {
    char_callback = callback;
}

void keyboard_set_special_callback(special_key_callback_t callback) {
    special_callback = callback;
} 