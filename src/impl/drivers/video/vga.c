/**
 * VGA Graphics Driver Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "vga.h"
#include "../port_io/port.h"
#include "../serial/serial.h"
#include "../../kernel/asm_utils.h"
#include <string.h>
#include <stdio.h>

/* Absolute value function */
static int abs(int x) {
    return x < 0 ? -x : x;
}

/* Current VGA mode */
static struct VGAMode current_mode;

/* Double buffering */
static uint8_t* front_buffer = (uint8_t*)VGA_MEMORY_BASE;
static uint8_t back_buffer[VGA_MEMORY_SIZE];

/* Window system colors */
#define WINDOW_BORDER_COLOR     VGA_COLOR_LIGHT_GRAY
#define WINDOW_TITLE_COLOR      VGA_COLOR_BLUE
#define WINDOW_BACKGROUND_COLOR VGA_COLOR_WHITE
#define TASKBAR_COLOR          VGA_COLOR_DARK_GRAY
#define DESKTOP_COLOR          VGA_COLOR_CYAN

/* Initialize VGA */
void vga_init(void) {
    serial_write_string(COM1_PORT, "[VGA] Initializing graphics system...\n");
    
    /* Initialize VGA registers */
    port_byte_out(0x3C2, 0x63);  /* Miscellaneous output register */
    
    /* Sequencer registers */
    port_byte_out(0x3C4, 0x00);  /* Reset sequencer */
    port_byte_out(0x3C5, 0x03);
    port_byte_out(0x3C4, 0x01);  /* Clocking mode */
    port_byte_out(0x3C5, 0x01);
    
    /* Unlock CRTC registers */
    port_byte_out(0x3D4, 0x11);
    port_byte_out(0x3D5, port_byte_in(0x3D5) & 0x7F);
    
    /* Start in text mode */
    vga_set_mode(VGA_MODE_TEXT_80x25);
    
    /* Initialize buffers */
    memset(back_buffer, 0, VGA_MEMORY_SIZE);
    memset((void*)VGA_MEMORY_BASE, 0, VGA_MEMORY_SIZE);
    
    serial_write_string(COM1_PORT, "[VGA] Graphics system initialized\n");
}

/* Set VGA mode */
int vga_set_mode(uint8_t mode) {
    /* Basic VGA mode setup */
    port_byte_out(0x3C2, 0x63);  /* Miscellaneous output register */
    
    /* Sequencer registers */
    port_byte_out(0x3C4, 0x00);  /* Reset sequencer */
    port_byte_out(0x3C5, 0x03);
    port_byte_out(0x3C4, 0x01);  /* Clocking mode */
    port_byte_out(0x3C5, 0x01);
    
    /* Unlock CRTC registers */
    port_byte_out(0x3D4, 0x11);
    port_byte_out(0x3D5, port_byte_in(0x3D5) & 0x7F);
    
    /* Update current mode information */
    switch(mode) {
        case VGA_MODE_TEXT_80x25:
            current_mode.width = 80;
            current_mode.height = 25;
            current_mode.bpp = 4;
            current_mode.pitch = current_mode.width * 2;  /* 2 bytes per character */
            break;
            
        case VGA_MODE_320x200x256:
            current_mode.width = 320;
            current_mode.height = 200;
            current_mode.bpp = 8;
            current_mode.pitch = current_mode.width;
            break;
            
        case VGA_MODE_640x480x16:
            current_mode.width = 640;
            current_mode.height = 480;
            current_mode.bpp = 4;
            current_mode.pitch = current_mode.width / 2;  /* 4 bits per pixel */
            break;
            
        default:
            serial_write_string(COM1_PORT, "[VGA] Error: Unsupported video mode\n");
            return -1;
    }
    
    current_mode.mode = mode;
    current_mode.framebuffer = VGA_MEMORY_BASE;
    
    /* Clear both buffers */
    memset(back_buffer, 0, VGA_MEMORY_SIZE);
    memset((void*)VGA_MEMORY_BASE, 0, VGA_MEMORY_SIZE);
    
    serial_write_string(COM1_PORT, "[VGA] Video mode set successfully\n");
    return 0;
}

/* Set a pixel in the back buffer */
void vga_set_pixel(uint16_t x, uint16_t y, uint8_t color) {
    if (x >= current_mode.width || y >= current_mode.height) {
        return;
    }
    
    uint32_t offset = y * current_mode.pitch + x;
    back_buffer[offset] = color;
}

/* Draw a filled rectangle */
void vga_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            vga_set_pixel(x + j, y + i, color);
        }
    }
}

/* Draw a line using Bresenham's algorithm */
void vga_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    
    while (1) {
        vga_set_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }
}

/* Clear the screen */
void vga_clear_screen(uint8_t color) {
    memset(back_buffer, color, VGA_MEMORY_SIZE);
}

/* Swap front and back buffers */
void vga_swap_buffers(void) {
    memcpy(front_buffer, back_buffer, VGA_MEMORY_SIZE);
}

/* Draw text in graphics mode */
void vga_draw_text(uint16_t x, uint16_t y, const char* text, uint8_t color) {
    /* TODO: Implement font rendering */
    /* For now, just draw placeholder rectangles */
    uint16_t char_width = 8;
    uint16_t char_height = 16;
    
    for (size_t i = 0; text[i] != '\0'; i++) {
        vga_draw_rect(x + i * char_width, y, char_width - 1, char_height, color);
    }
}

/* Draw a window with title bar */
void vga_draw_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* title) {
    /* Draw window border */
    vga_draw_rect(x, y, width, height, WINDOW_BORDER_COLOR);
    
    /* Draw title bar */
    vga_draw_rect(x + 1, y + 1, width - 2, 20, WINDOW_TITLE_COLOR);
    vga_draw_text(x + 5, y + 5, title, VGA_COLOR_WHITE);
    
    /* Draw window background */
    vga_draw_rect(x + 1, y + 21, width - 2, height - 22, WINDOW_BACKGROUND_COLOR);
    
    /* Draw close button */
    vga_draw_rect(x + width - 18, y + 4, 14, 14, VGA_COLOR_RED);
    vga_draw_text(x + width - 16, y + 6, "X", VGA_COLOR_WHITE);
}

/* Draw a button */
void vga_draw_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* text) {
    /* Draw button background */
    vga_draw_rect(x, y, width, height, VGA_COLOR_LIGHT_GRAY);
    
    /* Draw button border */
    vga_draw_line(x, y, x + width - 1, y, VGA_COLOR_WHITE);
    vga_draw_line(x, y, x, y + height - 1, VGA_COLOR_WHITE);
    vga_draw_line(x, y + height - 1, x + width - 1, y + height - 1, VGA_COLOR_DARK_GRAY);
    vga_draw_line(x + width - 1, y, x + width - 1, y + height - 1, VGA_COLOR_DARK_GRAY);
    
    /* Draw button text */
    uint16_t text_x = x + (width - strlen(text) * 8) / 2;
    uint16_t text_y = y + (height - 16) / 2;
    vga_draw_text(text_x, text_y, text, VGA_COLOR_BLACK);
}

/* Initialize desktop environment */
void vga_init_desktop(void) {
    /* Set graphics mode */
    vga_set_mode(VGA_MODE_640x480x16);
    
    /* Draw desktop background */
    vga_clear_screen(DESKTOP_COLOR);
    
    /* Draw taskbar */
    vga_draw_taskbar();
    
    /* Draw desktop icons */
    vga_draw_desktop_icons();
    
    /* Initial screen update */
    vga_swap_buffers();
}

/* Draw taskbar */
void vga_draw_taskbar(void) {
    /* Draw taskbar background */
    vga_draw_rect(0, current_mode.height - 30, current_mode.width, 30, TASKBAR_COLOR);
    
    /* Draw start button */
    vga_draw_button(5, current_mode.height - 25, 60, 20, "Start");
    
    /* Draw clock */
    vga_draw_text(current_mode.width - 50, current_mode.height - 20, "12:00", VGA_COLOR_WHITE);
}

/* Draw desktop icons */
void vga_draw_desktop_icons(void) {
    /* Draw "My Computer" icon */
    vga_draw_rect(20, 20, 32, 32, VGA_COLOR_YELLOW);
    vga_draw_text(10, 60, "My Computer", VGA_COLOR_BLACK);
    
    /* Draw "Documents" icon */
    vga_draw_rect(20, 100, 32, 32, VGA_COLOR_BLUE);
    vga_draw_text(10, 140, "Documents", VGA_COLOR_BLACK);
    
    /* Draw "Terminal" icon */
    vga_draw_rect(20, 180, 32, 32, VGA_COLOR_BLACK);
    vga_draw_text(10, 220, "Terminal", VGA_COLOR_BLACK);
}

/* Update mouse cursor position */
void vga_update_cursor(uint16_t x, uint16_t y) {
    /* Draw cursor (simple arrow) */
    vga_draw_line(x, y, x + 8, y + 8, VGA_COLOR_BLACK);
    vga_draw_line(x, y, x + 2, y + 8, VGA_COLOR_BLACK);
    vga_draw_line(x + 2, y + 8, x + 8, y + 8, VGA_COLOR_BLACK);
}

/* Get current mode info */
const struct VGAMode* vga_get_mode(void) {
    return &current_mode;
} 