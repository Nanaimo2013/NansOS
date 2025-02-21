/**
 * Window Manager
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* Maximum number of windows */
#define MAX_WINDOWS 16

/* Window flags */
#define WINDOW_FLAG_VISIBLE   0x01
#define WINDOW_FLAG_MOVABLE   0x02
#define WINDOW_FLAG_RESIZABLE 0x04
#define WINDOW_FLAG_MINIMIZED 0x08
#define WINDOW_FLAG_MAXIMIZED 0x10

/* Window structure */
struct Window {
    char title[32];
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint32_t flags;
    void* content;
    void (*on_paint)(struct Window* window);
    void (*on_click)(struct Window* window, uint16_t x, uint16_t y);
    void (*on_key)(struct Window* window, char key);
};

/* Window manager functions */
void window_init(void);
void window_init_gui(void);  /* Initialize graphical interface */
struct Window* window_create(const char* title, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void window_destroy(struct Window* window);
void window_show(struct Window* window);
void window_hide(struct Window* window);
void window_move(struct Window* window, uint16_t x, uint16_t y);
void window_resize(struct Window* window, uint16_t width, uint16_t height);
void window_bring_to_front(struct Window* window);
void window_minimize(struct Window* window);
void window_maximize(struct Window* window);
void window_restore(struct Window* window);

/* Window manager event handling */
void window_handle_mouse(uint16_t x, uint16_t y, uint8_t buttons);
void window_handle_key(char key);
void window_update(void);

/* Window content functions */
void window_draw_text(struct Window* window, uint16_t x, uint16_t y, const char* text);
void window_draw_button(struct Window* window, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* text);
void window_clear(struct Window* window, uint8_t color);

/* Built-in window types */
struct Window* window_create_terminal(void);
struct Window* window_create_file_browser(void);
struct Window* window_create_text_editor(void); 