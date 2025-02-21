/**
 * Window Manager Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "window.h"
#include "vga.h"
#include "../serial/serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Window system colors */
#define DESKTOP_COLOR          VGA_COLOR_CYAN
#define WINDOW_BORDER_COLOR    VGA_COLOR_LIGHT_GRAY
#define WINDOW_TITLE_COLOR     VGA_COLOR_BLUE
#define WINDOW_BACKGROUND_COLOR VGA_COLOR_WHITE
#define TASKBAR_COLOR         VGA_COLOR_DARK_GRAY

/* Window list */
static struct Window* windows[MAX_WINDOWS];
static int num_windows = 0;
static struct Window* active_window = NULL;
static struct Window* dragging_window = NULL;
static int drag_offset_x = 0;
static int drag_offset_y = 0;

/* Terminal buffer */
#define TERMINAL_BUFFER_SIZE 1024
struct TerminalBuffer {
    char data[TERMINAL_BUFFER_SIZE];
    int cursor;
};

/* Initialize window manager */
void window_init(void) {
    serial_write_string(COM1_PORT, "[WINDOW] Initializing window manager...\n");
    
    /* Initialize window list */
    memset(windows, 0, sizeof(windows));
    num_windows = 0;
    active_window = NULL;
    dragging_window = NULL;
    
    /* Initialize in text mode first */
    vga_init();
    
    serial_write_string(COM1_PORT, "[WINDOW] Window manager initialized\n");
}

/* Create a new window */
struct Window* window_create(const char* title, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    if (num_windows >= MAX_WINDOWS) {
        serial_write_string(COM1_PORT, "[WINDOW] Error: Maximum number of windows reached\n");
        return NULL;
    }
    
    struct Window* window = (struct Window*)malloc(sizeof(struct Window));
    if (!window) {
        serial_write_string(COM1_PORT, "[WINDOW] Error: Failed to allocate window structure\n");
        return NULL;
    }
    
    /* Initialize window */
    strncpy(window->title, title, sizeof(window->title) - 1);
    window->title[sizeof(window->title) - 1] = '\0';
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->flags = WINDOW_FLAG_VISIBLE | WINDOW_FLAG_MOVABLE;
    window->content = NULL;
    window->on_paint = NULL;
    window->on_click = NULL;
    window->on_key = NULL;
    
    /* Add to window list */
    windows[num_windows++] = window;
    
    /* Make it the active window */
    window_bring_to_front(window);
    
    return window;
}

/* Destroy a window */
void window_destroy(struct Window* window) {
    if (!window) return;
    
    /* Remove from window list */
    for (int i = 0; i < num_windows; i++) {
        if (windows[i] == window) {
            /* Shift remaining windows */
            for (int j = i; j < num_windows - 1; j++) {
                windows[j] = windows[j + 1];
            }
            windows[--num_windows] = NULL;
            
            /* Update active window if needed */
            if (active_window == window) {
                active_window = (num_windows > 0) ? windows[num_windows - 1] : NULL;
            }
            
            /* Free window resources */
            if (window->content) {
                free(window->content);
            }
            free(window);
            break;
        }
    }
}

/* Show a window */
void window_show(struct Window* window) {
    if (!window) return;
    window->flags |= WINDOW_FLAG_VISIBLE;
    window_bring_to_front(window);
}

/* Hide a window */
void window_hide(struct Window* window) {
    if (!window) return;
    window->flags &= ~WINDOW_FLAG_VISIBLE;
    if (active_window == window) {
        active_window = (num_windows > 0) ? windows[num_windows - 1] : NULL;
    }
}

/* Move a window */
void window_move(struct Window* window, uint16_t x, uint16_t y) {
    if (!window) return;
    window->x = x;
    window->y = y;
}

/* Resize a window */
void window_resize(struct Window* window, uint16_t width, uint16_t height) {
    if (!window || !(window->flags & WINDOW_FLAG_RESIZABLE)) return;
    window->width = width;
    window->height = height;
}

/* Bring window to front */
void window_bring_to_front(struct Window* window) {
    if (!window) return;
    
    /* Find window in list */
    for (int i = 0; i < num_windows; i++) {
        if (windows[i] == window) {
            /* Move to end of list */
            for (int j = i; j < num_windows - 1; j++) {
                windows[j] = windows[j + 1];
            }
            windows[num_windows - 1] = window;
            active_window = window;
            break;
        }
    }
}

/* Minimize window */
void window_minimize(struct Window* window) {
    if (!window) return;
    window->flags |= WINDOW_FLAG_MINIMIZED;
    window->flags &= ~WINDOW_FLAG_MAXIMIZED;
}

/* Maximize window */
void window_maximize(struct Window* window) {
    if (!window) return;
    window->flags |= WINDOW_FLAG_MAXIMIZED;
    window->flags &= ~WINDOW_FLAG_MINIMIZED;
}

/* Restore window */
void window_restore(struct Window* window) {
    if (!window) return;
    window->flags &= ~(WINDOW_FLAG_MINIMIZED | WINDOW_FLAG_MAXIMIZED);
}

/* Handle mouse input */
void window_handle_mouse(uint16_t x, uint16_t y, uint8_t buttons) {
    /* Update cursor */
    vga_update_cursor(x, y);
    
    /* Handle window dragging */
    if (dragging_window) {
        if (buttons & 0x01) {  /* Left button still held */
            window_move(dragging_window,
                       x - drag_offset_x,
                       y - drag_offset_y);
        } else {
            dragging_window = NULL;
        }
        return;
    }
    
    /* Check for window interactions */
    for (int i = num_windows - 1; i >= 0; i--) {
        struct Window* window = windows[i];
        if (!(window->flags & WINDOW_FLAG_VISIBLE)) continue;
        
        /* Check if click is in window */
        if (x >= window->x && x < window->x + window->width &&
            y >= window->y && y < window->y + window->height) {
            
            /* Check for title bar click */
            if (y < window->y + 20) {
                if (buttons & 0x01) {  /* Left button */
                    /* Start dragging */
                    dragging_window = window;
                    drag_offset_x = x - window->x;
                    drag_offset_y = y - window->y;
                    window_bring_to_front(window);
                }
                /* Check for close button */
                if (x >= window->x + window->width - 18 &&
                    x < window->x + window->width - 4 &&
                    y >= window->y + 4 && y < window->y + 18) {
                    window_destroy(window);
                }
            } else if (window->on_click) {
                window->on_click(window, x - window->x, y - window->y);
            }
            break;
        }
    }
}

/* Handle keyboard input */
void window_handle_key(char key) {
    if (active_window && active_window->on_key) {
        active_window->on_key(active_window, key);
    }
}

/* Update all windows */
void window_update(void) {
    /* Clear screen */
    vga_clear_screen(DESKTOP_COLOR);
    
    /* Draw taskbar */
    vga_draw_taskbar();
    
    /* Draw all windows */
    for (int i = 0; i < num_windows; i++) {
        struct Window* window = windows[i];
        if (!(window->flags & WINDOW_FLAG_VISIBLE)) continue;
        
        /* Draw window frame */
        vga_draw_window(window->x, window->y, window->width, window->height, window->title);
        
        /* Draw window content */
        if (window->on_paint) {
            window->on_paint(window);
        }
    }
    
    /* Update screen */
    vga_swap_buffers();
}

/* Window content functions */
void window_draw_text(struct Window* window, uint16_t x, uint16_t y, const char* text) {
    if (!window) return;
    vga_draw_text(window->x + x, window->y + y + 20, text, VGA_COLOR_BLACK);
}

void window_draw_button(struct Window* window, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* text) {
    if (!window) return;
    vga_draw_button(window->x + x, window->y + y + 20, width, height, text);
}

void window_clear(struct Window* window, uint8_t color) {
    if (!window) return;
    vga_draw_rect(window->x + 1, window->y + 21,
                  window->width - 2, window->height - 22,
                  color);
}

/* Terminal window */
static void terminal_paint(struct Window* window) {
    struct TerminalBuffer* buffer = window->content;
    if (buffer) {
        window_clear(window, VGA_COLOR_BLACK);
        window_draw_text(window, 2, 2, buffer->data);
    }
}

static void terminal_key(struct Window* window, char key) {
    struct TerminalBuffer* buffer = window->content;
    if (buffer && buffer->cursor < TERMINAL_BUFFER_SIZE - 1) {
        buffer->data[buffer->cursor++] = key;
        buffer->data[buffer->cursor] = '\0';
    }
}

struct Window* window_create_terminal(void) {
    struct Window* window = window_create("Terminal", 100, 100, 400, 300);
    if (!window) return NULL;

    /* Allocate terminal buffer */
    struct TerminalBuffer* buffer = malloc(sizeof(struct TerminalBuffer));
    if (!buffer) {
        window_destroy(window);
        return NULL;
    }

    /* Initialize buffer */
    memset(buffer, 0, sizeof(struct TerminalBuffer));
    window->content = buffer;
    window->on_paint = terminal_paint;
    window->on_key = terminal_key;

    return window;
}

/* File browser window */
static void file_browser_paint(struct Window* window) {
    window_clear(window, VGA_COLOR_WHITE);
    window_draw_text(window, 2, 2, "File Browser - Coming Soon!");
}

struct Window* window_create_file_browser(void) {
    struct Window* window = window_create("File Browser", 150, 150, 500, 400);
    if (!window) return NULL;

    window->on_paint = file_browser_paint;
    return window;
}

/* Text editor window */
static void text_editor_paint(struct Window* window) {
    window_clear(window, VGA_COLOR_WHITE);
    window_draw_text(window, 2, 2, "Text Editor - Coming Soon!");
}

struct Window* window_create_text_editor(void) {
    struct Window* window = window_create("Text Editor", 200, 200, 600, 450);
    if (!window) return NULL;

    window->on_paint = text_editor_paint;
    return window;
}

/* Initialize graphical interface */
void window_init_gui(void) {
    serial_write_string(COM1_PORT, "[WINDOW] Initializing graphical interface...\n");
    
    /* Get current VGA mode */
    const struct VGAMode* mode = vga_get_mode();
    if (!mode) {
        serial_write_string(COM1_PORT, "[WINDOW] Error: No VGA mode set\n");
        return;
    }
    
    /* Set up graphics mode */
    vga_set_mode(VGA_MODE_640x480x16);
    
    /* Initialize desktop environment */
    vga_init_desktop();
    
    /* Create initial windows */
    struct Window* terminal = window_create_terminal();
    if (terminal) {
        window_show(terminal);
        window_move(terminal, 50, 50);
    }
    
    struct Window* file_browser = window_create_file_browser();
    if (file_browser) {
        window_show(file_browser);
        window_move(file_browser, 200, 100);
    }
    
    /* Initial screen update */
    vga_swap_buffers();
    
    serial_write_string(COM1_PORT, "[WINDOW] Graphical interface initialized\n");
} 