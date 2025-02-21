/**
 * PS/2 Mouse Driver Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "mouse.h"
#include "../port_io/port.h"
#include "../pic/pic.h"
#include "../serial/serial.h"
#include "../../kernel/asm_utils.h"
#include "../keyboard/keyboard.h"

/* Mouse IRQ number */
#define MOUSE_IRQ 12

/* Internal variables */
struct MouseState mouse_state = {
    .x_pos = 512,  /* Start at center of screen */
    .y_pos = 384,
    .buttons = 0,
    .scroll = 0
};
static mouse_callback_t mouse_callback = 0;

void mouse_init(void) {
    serial_write_string(COM1_PORT, "[MOUSE] Starting mouse initialization...\n");
    
    /* Disable interrupts during initialization */
    asm_cli();
    
    /* Step 1: Disable both devices */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xAD);  /* Disable keyboard */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xA7);  /* Disable mouse */
    
    /* Step 2: Flush the output buffer */
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 1)) {
        port_byte_in(KEYBOARD_DATA_PORT);
    }
    
    /* Step 3: Set the controller configuration byte */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0x20);  /* Read config */
    
    int timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            uint8_t config = port_byte_in(KEYBOARD_DATA_PORT);
            config |= 0x02;      /* Enable IRQ12 */
            config &= ~0x20;     /* Enable mouse clock */
            config |= 0x01;      /* Enable keyboard interrupt */
            config &= ~0x10;     /* Enable keyboard clock */
            
            port_byte_out(KEYBOARD_COMMAND_PORT, 0x60);  /* Write config */
            port_byte_out(KEYBOARD_DATA_PORT, config);
            serial_write_string(COM1_PORT, "[MOUSE] Controller configured\n");
            break;
        }
    }
    if (timeout == 0) {
        serial_write_string(COM1_PORT, "[MOUSE] Error: Controller configuration failed\n");
        asm_sti();
        return;
    }
    
    /* Step 4: Enable the auxiliary mouse device */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xA8);
    
    /* Small delay */
    for (volatile int i = 0; i < 100; i++) {}
    
    /* Step 5: Set mouse parameters */
    /* Set sample rate */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xD4);
    port_byte_out(KEYBOARD_DATA_PORT, 0xF3);  /* Set sample rate */
    port_byte_out(KEYBOARD_DATA_PORT, 100);   /* 100 samples/sec */
    
    /* Set resolution */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xD4);
    port_byte_out(KEYBOARD_DATA_PORT, 0xE8);  /* Set resolution */
    port_byte_out(KEYBOARD_DATA_PORT, 0x03);  /* 8 counts/mm */
    
    /* Set scaling 1:1 */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xD4);
    port_byte_out(KEYBOARD_DATA_PORT, 0xE6);
    
    /* Enable packet streaming */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xD4);
    port_byte_out(KEYBOARD_DATA_PORT, 0xF4);
    
    timeout = 100000;
    while (--timeout > 0) {
        if (port_byte_in(KEYBOARD_STATUS_PORT) & 1) {
            if (port_byte_in(KEYBOARD_DATA_PORT) == 0xFA) {
                serial_write_string(COM1_PORT, "[MOUSE] Packet streaming enabled\n");
                break;
            }
        }
    }
    
    /* Step 6: Re-enable keyboard */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xAE);
    
    /* Initialize mouse state */
    mouse_state.x_pos = 512;  /* Start at center of screen */
    mouse_state.y_pos = 384;
    mouse_state.buttons = 0;
    
    /* Enable mouse interrupt */
    pic_clear_mask(MOUSE_IRQ);
    serial_write_string(COM1_PORT, "[MOUSE] Interrupt enabled\n");
    
    /* Re-enable interrupts */
    asm_sti();
    
    serial_write_string(COM1_PORT, "[MOUSE] Initialization complete\n");
}

void mouse_handler(void) {
    static uint8_t cycle = 0;
    static struct MousePacket packet = {0};
    uint8_t status;
    char debug_msg[64];
    
    /* Read status before data */
    status = port_byte_in(KEYBOARD_STATUS_PORT);
    
    /* Debug output */
    sprintf(debug_msg, "[MOUSE] Status: 0x%02x\n", status);
    serial_write_string(COM1_PORT, debug_msg);
    
    /* Check if this is actually mouse data */
    if (!(status & 0x20)) {
        serial_write_string(COM1_PORT, "[MOUSE] Not mouse data, ignoring\n");
        pic_send_eoi(MOUSE_IRQ);
        return;
    }
    
    /* Read the data */
    uint8_t data = port_byte_in(KEYBOARD_DATA_PORT);
    
    /* Debug output */
    sprintf(debug_msg, "[MOUSE] Received data: 0x%02x, cycle: %d\n", data, cycle);
    serial_write_string(COM1_PORT, debug_msg);
    
    switch(cycle) {
        case 0:
            /* Verify first byte (should have bit 3 set) */
            if (data & 0x08) {
                packet.buttons = data;
                cycle = 1;
                serial_write_string(COM1_PORT, "[MOUSE] First byte received\n");
            } else {
                serial_write_string(COM1_PORT, "[MOUSE] Invalid first byte, resetting\n");
            }
            break;
            
        case 1:
            packet.x_movement = (int8_t)data;
            cycle = 2;
            serial_write_string(COM1_PORT, "[MOUSE] X movement received\n");
            break;
            
        case 2:
            packet.y_movement = (int8_t)data;
            serial_write_string(COM1_PORT, "[MOUSE] Y movement received\n");
            
            /* Update mouse state */
            mouse_state.buttons = packet.buttons & 0x07;
            
            /* Update position */
            int32_t new_x = mouse_state.x_pos;
            int32_t new_y = mouse_state.y_pos;
            
            /* Handle X movement */
            new_x += packet.x_movement;
            
            /* Handle Y movement (Y is inverted) */
            new_y -= packet.y_movement;
            
            /* Clamp values to screen boundaries */
            mouse_state.x_pos = (new_x < 0) ? 0 : (new_x > 1023) ? 1023 : new_x;
            mouse_state.y_pos = (new_y < 0) ? 0 : (new_y > 767) ? 767 : new_y;
            
            /* Debug output */
            sprintf(debug_msg, "[MOUSE] Position: X=%d, Y=%d, Buttons=%02x\n", 
                    mouse_state.x_pos, mouse_state.y_pos, mouse_state.buttons);
            serial_write_string(COM1_PORT, debug_msg);
            
            /* Call callback if registered */
            if (mouse_callback) {
                mouse_callback(&mouse_state);
                serial_write_string(COM1_PORT, "[MOUSE] Callback executed\n");
            } else {
                serial_write_string(COM1_PORT, "[MOUSE] No callback registered\n");
            }
            
            cycle = 0;
            break;
    }
    
    /* Send EOI to PIC */
    pic_send_eoi(MOUSE_IRQ);
}

void mouse_enable(void) {
    /* Wait for keyboard controller to be ready */
    int timeout = 10000;
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 2) != 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        return;
    }
    
    /* Send enable command */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xD4);
    port_byte_out(KEYBOARD_DATA_PORT, MOUSE_CMD_ENABLE);
    
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

void mouse_disable(void) {
    /* Wait for keyboard controller to be ready */
    int timeout = 10000;
    while ((port_byte_in(KEYBOARD_STATUS_PORT) & 2) != 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        return;
    }
    
    /* Send disable command */
    port_byte_out(KEYBOARD_COMMAND_PORT, 0xD4);
    port_byte_out(KEYBOARD_DATA_PORT, MOUSE_CMD_DISABLE);
    
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

void mouse_set_callback(mouse_callback_t callback) {
    mouse_callback = callback;
} 