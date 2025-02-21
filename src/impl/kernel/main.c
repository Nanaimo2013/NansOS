/**
 * NansOS Kernel Main
 * Copyright (c) 2025 NansStudios
 * 
 * This is the main kernel entry point for NansOS. It initializes all core
 * system components and sets up the basic operating environment.
 */

#include "../../intf/print.h"
#include "../../intf/stdio.h"
#include "../../intf/string.h"
#include "idt.h"
#include "sysinfo.h"
#include "mmu.h"
#include "asm_utils.h"
#include "../drivers/pic/pic.h"
#include "../drivers/pit/pit.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/mouse/mouse.h"
#include "../drivers/serial/serial.h"
#include "../drivers/rtc/rtc.h"
#include "../../impl/drivers/video/vga.h"
#include "../../impl/drivers/video/window.h"

/* Global system information */
static struct SystemInfo* system_info;

/* Current VGA mode */
static struct VGAMode current_mode;

/* Mouse state */
extern struct MouseState mouse_state;

/* Debug flags */
static int debug_mode = 1;

/* Test mode flag */
#ifdef TEST_MODE
static int test_mode = 1;
#else
static int test_mode = 0;
#endif

/* External test runner */
extern void run_tests(void);

/* Debug print function */
static void debug_print(const char* msg) {
    if (debug_mode) {
        print_set_color(PRINT_COLOR_CYAN, PRINT_COLOR_BLACK);
        print_str("[DEBUG] ");
        print_str(msg);
        print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
        serial_write_string(COM1_PORT, "[DEBUG] ");
        serial_write_string(COM1_PORT, msg);
    }
}

/* Function declarations */
static int init_system(void);
static void print_boot_header(void);
static void handle_system_error(const char* error_msg);

/* System configuration structure */
struct SystemConfig {
    char language[32];
    int screen_width;
    int screen_height;
    int color_depth;
    int keyboard_layout;
    int timezone;
    int storage_mode;  // 0 = auto, 1 = AHCI, 2 = IDE, 3 = NVMe
};

static struct SystemConfig system_config;
static int setup_complete = 0;

/* Setup wizard states */
enum SetupState {
    SETUP_WELCOME,
    SETUP_LANGUAGE,
    SETUP_DISPLAY,
    SETUP_KEYBOARD,
    SETUP_STORAGE,
    SETUP_TIMEZONE,
    SETUP_CONFIRM,
    SETUP_COMPLETE
};

static enum SetupState current_setup_state = SETUP_WELCOME;

/* Run the setup wizard */
static void run_setup_wizard(void) {
    print_clear();
    print_set_color(PRINT_COLOR_CYAN, PRINT_COLOR_BLACK);
    print_str("\n=== NansOS Setup Wizard ===\n\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);

    switch(current_setup_state) {
        case SETUP_WELCOME:
            print_str("Welcome to NansOS!\n");
            print_str("This wizard will help you configure your system.\n");
            print_str("Press ENTER to continue...\n");
            break;

        case SETUP_LANGUAGE:
            print_str("Select your language:\n");
            print_str("1. English\n");
            print_str("2. Spanish\n");
            print_str("3. French\n");
            print_str("4. German\n");
            print_str("Enter selection (1-4): ");
            break;

        case SETUP_DISPLAY:
            print_str("Select display resolution:\n");
            print_str("1. 800x600\n");
            print_str("2. 1024x768\n");
            print_str("3. 1280x720\n");
            print_str("4. 1920x1080\n");
            print_str("Enter selection (1-4): ");
            break;

        case SETUP_KEYBOARD:
            print_str("Select keyboard layout:\n");
            print_str("1. US (QWERTY)\n");
            print_str("2. UK (QWERTY)\n");
            print_str("3. German (QWERTZ)\n");
            print_str("4. French (AZERTY)\n");
            print_str("Enter selection (1-4): ");
            break;

        case SETUP_STORAGE:
            print_str("Configure storage:\n");
            print_str("1. Auto-detect (Recommended)\n");
            print_str("2. AHCI Mode\n");
            print_str("3. IDE Mode\n");
            print_str("4. NVMe Mode\n");
            print_str("Enter selection (1-4): ");
            break;

        case SETUP_TIMEZONE:
            print_str("Select timezone:\n");
            print_str("1. UTC-8 (Pacific Time)\n");
            print_str("2. UTC-5 (Eastern Time)\n");
            print_str("3. UTC+0 (GMT)\n");
            print_str("4. UTC+1 (Central European Time)\n");
            print_str("Enter selection (1-4): ");
            break;

        case SETUP_CONFIRM:
            print_str("Configuration Summary:\n");
            print_str("Language: "); print_str(system_config.language); print_str("\n");
            print_str("Display: "); 
            char res[32];
            sprintf(res, "%dx%d", system_config.screen_width, system_config.screen_height);
            print_str(res); print_str("\n");
            print_str("\nPress ENTER to confirm and start NansOS...\n");
            break;

        case SETUP_COMPLETE:
            setup_complete = 1;
            print_str("Setup complete! Starting NansOS...\n");
            break;
    }
}

/* Handle setup wizard input */
static void handle_setup_input(char c) {
    if (c == '\n' || c == '\r') {
        if (current_setup_state == SETUP_WELCOME) {
            current_setup_state = SETUP_LANGUAGE;
            run_setup_wizard();
            return;
        }
        if (current_setup_state == SETUP_CONFIRM) {
            current_setup_state = SETUP_COMPLETE;
            run_setup_wizard();
            return;
        }
    }

    if (c >= '1' && c <= '4') {
        switch(current_setup_state) {
            case SETUP_WELCOME:
            case SETUP_CONFIRM:
            case SETUP_COMPLETE:
                /* These states are handled above */
                break;

            case SETUP_LANGUAGE:
                strcpy(system_config.language, c == '1' ? "English" :
                                             c == '2' ? "Spanish" :
                                             c == '3' ? "French" : "German");
                current_setup_state = SETUP_DISPLAY;
                break;

            case SETUP_DISPLAY:
                switch(c) {
                    case '1': 
                        system_config.screen_width = 800;
                        system_config.screen_height = 600;
                        break;
                    case '2':
                        system_config.screen_width = 1024;
                        system_config.screen_height = 768;
                        break;
                    case '3':
                        system_config.screen_width = 1280;
                        system_config.screen_height = 720;
                        break;
                    case '4':
                        system_config.screen_width = 1920;
                        system_config.screen_height = 1080;
                        break;
                }
                current_setup_state = SETUP_KEYBOARD;
                break;

            case SETUP_KEYBOARD:
                system_config.keyboard_layout = c - '1';
                current_setup_state = SETUP_STORAGE;
                break;

            case SETUP_STORAGE:
                system_config.storage_mode = c - '1';
                current_setup_state = SETUP_TIMEZONE;
                break;

            case SETUP_TIMEZONE:
                system_config.timezone = c - '1';
                current_setup_state = SETUP_CONFIRM;
                break;
        }
        run_setup_wizard();
    }
}

/* Keyboard scancode callback */
void keyboard_scancode(uint8_t scancode) {
    char str[32];
    sprintf(str, "Scancode: 0x%02x\n", scancode);
    serial_write_string(COM1_PORT, str);
}

/* Modified keyboard character callback */
void keyboard_char(char c) {
    if (!setup_complete) {
        handle_setup_input(c);
        if (setup_complete) {
            /* Transition to graphical mode */
            print_str("\nInitializing graphical interface...\n");
            
            /* Initialize window system first */
            window_init();
            
            /* Small delay to ensure initialization is complete */
            for (volatile int i = 0; i < 1000000; i++) {}
            
            /* Now initialize GUI */
            window_init_gui();
            
            print_str("Welcome to NansOS Desktop!\n");
            serial_write_string(COM1_PORT, "Transitioned to graphical mode\n");
        }
    } else {
        /* Handle input in graphical mode */
        window_handle_key(c);
    }
}

/* Special key callback */
void special_key(struct SpecialKeyEvent* event) {
    char str[64];
    const char* key_name = "Unknown";
    
    /* Get key name */
    switch(event->key_code) {
        case KEY_F1: key_name = "F1"; break;
        case KEY_F2: key_name = "F2"; break;
        case KEY_F3: key_name = "F3"; break;
        case KEY_F4: key_name = "F4"; break;
        case KEY_F5: key_name = "F5"; break;
        case KEY_F6: key_name = "F6"; break;
        case KEY_F7: key_name = "F7"; break;
        case KEY_F8: key_name = "F8"; break;
        case KEY_F9: key_name = "F9"; break;
        case KEY_F10: key_name = "F10"; break;
        case KEY_F11: key_name = "F11"; break;
        case KEY_F12: key_name = "F12"; break;
        case KEY_UP: key_name = "Up"; break;
        case KEY_DOWN: key_name = "Down"; break;
        case KEY_LEFT: key_name = "Left"; break;
        case KEY_RIGHT: key_name = "Right"; break;
        case KEY_HOME: key_name = "Home"; break;
        case KEY_END: key_name = "End"; break;
        case KEY_PAGEUP: key_name = "PgUp"; break;
        case KEY_PAGEDOWN: key_name = "PgDn"; break;
        case KEY_INSERT: key_name = "Insert"; break;
        case KEY_DELETE: key_name = "Delete"; break;
        case KEY_ESCAPE: key_name = "Escape"; break;
    }

    /* Handle special key functions */
    if (event->is_pressed) {
        if (event->key_code == KEY_F1) {
            sysinfo_print_all();
        } else if (event->key_code == KEY_F2) {
            sysinfo_detect_memory(&system_info->memory);
            print_str("\nMemory information updated.\n");
        } else if (event->key_code == KEY_F3) {
            print_str("\nCPU Features:\n");
            if (system_info->cpu.features & CPU_FEATURE_FPU) print_str("FPU ");
            if (system_info->cpu.features & CPU_FEATURE_MMX) print_str("MMX ");
            if (system_info->cpu.features & CPU_FEATURE_SSE) print_str("SSE ");
            if (system_info->cpu.features & CPU_FEATURE_SSE2) print_str("SSE2 ");
            if (system_info->cpu.features & CPU_FEATURE_SSE3) print_str("SSE3 ");
            if (system_info->cpu.features & CPU_FEATURE_AVX) print_str("AVX ");
            if (system_info->cpu.features & CPU_FEATURE_VMX) print_str("VMX ");
            print_str("\n");
        }
    }
    
    /* Format message */
    sprintf(str, "Special Key: %s (%s) [Ctrl:%d Shift:%d Alt:%d]\n",
            key_name,
            event->is_pressed ? "Pressed" : "Released",
            event->state.ctrl_pressed,
            event->state.shift_pressed,
            event->state.alt_pressed);
            
    /* Print to both screen and serial */
    print_str(str);
    serial_write_string(COM1_PORT, str);
}

/* Mouse event handler */
void mouse_event(struct MouseState* state) {
    char debug_msg[64];
    sprintf(debug_msg, "[MOUSE] Event: X=%d, Y=%d, Buttons=%02x\n",
            state->x_pos, state->y_pos, state->buttons);
    serial_write_string(COM1_PORT, debug_msg);
    
    /* TODO: Update cursor position in GUI when implemented */
}

/* Timer callback function */
void timer_tick(void) {
    static uint32_t ticks = 0;
    ticks++;
    
    if (ticks % 100 == 0) {  /* Every second (assuming 100Hz PIT) */
        char str[64];
        sprintf(str, "Uptime: %d seconds\n", ticks / 100);
        serial_write_string(COM1_PORT, str);
    }
}

/* Print boot header with system information */
static void print_boot_header(void) {
    print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
    print_str("\n=================================\n");
    print_str("       NansOS v0.1.0\n");
    print_str("  Copyright (c) 2025 NansStudios\n");
    print_str("=================================\n\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
}

/* Initialize all system components */
static int init_system(void) {
    debug_print("Starting system initialization...\n");

    /* Initialize memory management */
    debug_print("Initializing memory management...\n");
    mmu_init();
    vmm_init();
    debug_print("Memory management initialized\n");

    /* Initialize system information */
    debug_print("Initializing system information...\n");
    sysinfo_init();
    debug_print("System information initialized\n");

    /* Initialize IDT */
    debug_print("Initializing IDT...\n");
    idt_init();
    debug_print("IDT initialized\n");

    /* Initialize PIC */
    debug_print("Initializing PIC...\n");
    pic_init();
    debug_print("PIC initialized\n");

    /* Initialize PIT */
    debug_print("Initializing PIT...\n");
    pit_init(PIT_DEFAULT_HZ);
    pit_set_callback(timer_tick);
    debug_print("PIT initialized\n");

    /* Initialize serial port */
    debug_print("Initializing serial port...\n");
    serial_init(COM1_PORT, SERIAL_BAUD_115200);
    debug_print("Serial port initialized\n");

    /* Initialize RTC */
    debug_print("Initializing RTC...\n");
    rtc_init();
    rtc_enable_interrupt();
    debug_print("RTC initialized\n");

    /* Initialize keyboard */
    debug_print("Initializing keyboard...\n");
    keyboard_init();
    keyboard_set_scancode_callback(keyboard_scancode);
    keyboard_set_char_callback(keyboard_char);
    keyboard_set_special_callback(special_key);
    debug_print("Keyboard initialization complete\n");

    /* Initialize mouse with callback */
    debug_print("Initializing mouse...\n");
    mouse_init();
    mouse_set_callback(mouse_event);
    debug_print("Mouse initialization complete\n");

    /* Enable interrupts */
    debug_print("Enabling interrupts...\n");
    idt_enable_interrupts();
    debug_print("Interrupts enabled\n");

    debug_print("System initialization complete!\n");
    serial_write_string(COM1_PORT, "NansOS initialized and ready!\n");

    return 0; /* Success */
}

/* Handle system errors */
static void handle_system_error(const char* error_msg) {
    print_set_color(PRINT_COLOR_RED, PRINT_COLOR_BLACK);
    print_str("\nFATAL ERROR: ");
    print_str(error_msg);
    print_str("\nSystem halted.\n");
    
    serial_write_string(COM1_PORT, "\nFATAL ERROR: ");
    serial_write_string(COM1_PORT, error_msg);
    serial_write_string(COM1_PORT, "\nSystem halted.\n");
    
    /* Disable interrupts and halt */
    idt_disable_interrupts();
    for(;;) { asm_hlt(); }
}

/* Kernel entry point */
void kernel_main(void) {
    /* Disable interrupts during initialization */
    asm_cli();
    
    /* Initialize serial port first for debugging */
    serial_init(COM1_PORT, SERIAL_BAUD_115200);
    serial_write_string(COM1_PORT, "Serial port initialized\n");
    
    /* Clear screen and print header */
    print_clear();
    print_boot_header();
    serial_write_string(COM1_PORT, "Kernel started\n");
    
    /* Initialize system components */
    if (init_system() != 0) {
        handle_system_error("System initialization failed");
        return;
    }

    /* Main kernel loop */
    serial_write_string(COM1_PORT, "Entering main kernel loop\n");
    run_setup_wizard();  // Start the setup wizard

    while (1) {
        if (setup_complete) {
            /* Process any pending tasks */
            if (pit_get_ticks() % PIT_DEFAULT_HZ == 0) {
                /* Update clock in taskbar */
                char time_str[32];
                uint32_t seconds = pit_get_ticks() / PIT_DEFAULT_HZ;
                uint32_t minutes = seconds / 60;
                uint32_t hours = minutes / 60;
                sprintf(time_str, "%02d:%02d:%02d", hours % 24, minutes % 60, seconds % 60);
                
                /* Update window system */
                window_update();
                
                /* Draw time in taskbar */
                const struct VGAMode* mode = vga_get_mode();
                if (mode && mode->mode != VGA_MODE_TEXT_80x25) {
                    vga_draw_text(mode->width - 70, mode->height - 20, time_str, VGA_COLOR_WHITE);
                    vga_swap_buffers();
                }
            }
        }
        
        /* Process mouse events if in graphical mode */
        if (setup_complete) {
            /* Update cursor position */
            vga_update_cursor(mouse_state.x_pos, mouse_state.y_pos);
        }
        
        /* Halt CPU until next interrupt */
        asm_hlt();
    }
}
