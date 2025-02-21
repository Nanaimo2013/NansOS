/**
 * VGA Graphics Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* VGA ports */
#define VGA_AC_INDEX      0x3C0
#define VGA_AC_WRITE     0x3C0
#define VGA_AC_READ      0x3C1
#define VGA_MISC_WRITE   0x3C2
#define VGA_SEQ_INDEX    0x3C4
#define VGA_SEQ_DATA     0x3C5
#define VGA_DAC_WRITE    0x3C8
#define VGA_DAC_READ     0x3C7
#define VGA_DAC_DATA     0x3C9
#define VGA_MISC_READ    0x3CC
#define VGA_GC_INDEX     0x3CE
#define VGA_GC_DATA      0x3CF
#define VGA_CRTC_INDEX   0x3D4
#define VGA_CRTC_DATA    0x3D5
#define VGA_INSTAT_READ  0x3DA

/* VGA color definitions */
#define VGA_COLOR_BLACK          0x00
#define VGA_COLOR_BLUE          0x01
#define VGA_COLOR_GREEN         0x02
#define VGA_COLOR_CYAN          0x03
#define VGA_COLOR_RED           0x04
#define VGA_COLOR_MAGENTA       0x05
#define VGA_COLOR_BROWN         0x06
#define VGA_COLOR_LIGHT_GRAY    0x07
#define VGA_COLOR_DARK_GRAY     0x08
#define VGA_COLOR_LIGHT_BLUE    0x09
#define VGA_COLOR_LIGHT_GREEN   0x0A
#define VGA_COLOR_LIGHT_CYAN    0x0B
#define VGA_COLOR_LIGHT_RED     0x0C
#define VGA_COLOR_LIGHT_MAGENTA 0x0D
#define VGA_COLOR_YELLOW        0x0E
#define VGA_COLOR_WHITE         0x0F

/* Standard VGA modes */
#define VGA_MODE_TEXT_80x25     0x03
#define VGA_MODE_320x200x256    0x13
#define VGA_MODE_640x480x16     0x12
#define VGA_MODE_800x600x16     0x6A
#define VGA_MODE_1024x768x16    0x6B

/* VGA frame buffer */
#define VGA_MEMORY_BASE    0xA0000
#define VGA_MEMORY_SIZE    0x10000

/* VGA mode information */
struct VGAMode {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    uint8_t mode;
    uint32_t framebuffer;
    uint32_t pitch;
};

/* Function declarations */
void vga_init(void);
int vga_set_mode(uint8_t mode);
void vga_set_pixel(uint16_t x, uint16_t y, uint8_t color);
void vga_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
void vga_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);
void vga_clear_screen(uint8_t color);
void vga_swap_buffers(void);

/* Window system primitives */
void vga_draw_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* title);
void vga_draw_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* text);
void vga_draw_text(uint16_t x, uint16_t y, const char* text, uint8_t color);

/* Desktop environment */
void vga_init_desktop(void);
void vga_draw_taskbar(void);
void vga_draw_desktop_icons(void);
void vga_update_cursor(uint16_t x, uint16_t y);

/* Get current mode info */
const struct VGAMode* vga_get_mode(void); 