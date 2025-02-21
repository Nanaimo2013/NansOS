/**
 * Interrupt Service Routines
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "idt.h"
#include "asm_utils.h"
#include "../../intf/print.h"
#include <stdio.h>
#include "../drivers/pic/pic.h"
#include "../drivers/pit/pit.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/mouse/mouse.h"
#include "../drivers/serial/serial.h"
#include <stdint.h>

/* Helper function to print a hex number */
static void print_hex(uint64_t num) {
    char hex_str[17];
    int i;
    
    /* Convert to hex string */
    for (i = 15; i >= 0; i--) {
        int digit = num & 0xF;
        hex_str[i] = digit < 10 ? '0' + digit : 'A' + (digit - 10);
        num >>= 4;
    }
    hex_str[16] = '\0';
    
    print_str("0x");
    print_str(hex_str);
}

/* Default handler for unimplemented interrupts */
void isr_default_handler(struct InterruptFrame* frame) {
    (void)frame;  /* Unused parameter */
    serial_write_string(COM1_PORT, "Unhandled interrupt\n");
    pic_send_eoi(0);  /* Send EOI to both PICs */
    pic_send_eoi(8);
}

/* Timer interrupt handler */
void isr_timer_handler(struct InterruptFrame* frame) {
    (void)frame;  /* Unused parameter */
    pit_handler();  /* PIT handler will send EOI */
}

/* Keyboard interrupt handler */
void isr_keyboard_handler(struct InterruptFrame* frame) {
    (void)frame;  /* Unused parameter */
    keyboard_handler();  /* Keyboard handler will send EOI */
}

/* Page fault handler */
void isr_page_fault_handler(struct InterruptFrame* frame) {
    uint64_t fault_addr = asm_read_cr2();
    
    /* Print error message */
    print_str("Page Fault! Address: ");
    print_hex(fault_addr);
    print_str("\n");
    
    /* Send error to serial port */
    char error_msg[64];
    sprintf(error_msg, "Page Fault at address 0x%llx! System halted.\n", fault_addr);
    serial_write_string(COM1_PORT, error_msg);
    
    /* Disable interrupts and halt */
    asm_cli();
    for(;;) { asm_hlt(); }
}

/* Exception handlers */
void isr_divide_by_zero(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Divide by zero exception\n");
    asm_cli();
    for(;;) { asm_hlt(); }
}

void isr_debug(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Debug exception\n");
    pic_send_eoi(0);
}

void isr_nmi(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "NMI interrupt\n");
    pic_send_eoi(0);
}

void isr_breakpoint(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Breakpoint exception\n");
    pic_send_eoi(0);
}

void isr_overflow(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Overflow exception\n");
    pic_send_eoi(0);
}

void isr_bound_range(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Bound range exceeded\n");
    pic_send_eoi(0);
}

void isr_invalid_opcode(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Invalid opcode\n");
    asm_cli();
    for(;;) { asm_hlt(); }
}

void isr_device_not_available(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Device not available\n");
    pic_send_eoi(0);
}

void isr_double_fault(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Double fault\n");
    asm_cli();
    for(;;) { asm_hlt(); }
}

void isr_invalid_tss(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Invalid TSS\n");
    pic_send_eoi(0);
}

void isr_segment_not_present(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Segment not present\n");
    pic_send_eoi(0);
}

void isr_stack_segment(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Stack segment fault\n");
    pic_send_eoi(0);
}

void isr_general_protection(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "General protection fault\n");
    
    /* Print error information */
    if (frame->error_code != 0) {
        char error_msg[64];
        sprintf(error_msg, "Error code: 0x%llx\n", frame->error_code);
        serial_write_string(COM1_PORT, error_msg);
        sprintf(error_msg, "RIP: 0x%llx\n", frame->rip);
        serial_write_string(COM1_PORT, error_msg);
        sprintf(error_msg, "CS: 0x%llx\n", frame->cs);
        serial_write_string(COM1_PORT, error_msg);
        sprintf(error_msg, "RFLAGS: 0x%llx\n", frame->rflags);
        serial_write_string(COM1_PORT, error_msg);
        sprintf(error_msg, "RSP: 0x%llx\n", frame->rsp);
        serial_write_string(COM1_PORT, error_msg);
        sprintf(error_msg, "SS: 0x%llx\n", frame->ss);
        serial_write_string(COM1_PORT, error_msg);
    }
    
    asm_cli();
    for(;;) { asm_hlt(); }
}

void isr_fpu_fault(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "FPU fault\n");
    pic_send_eoi(0);
}

void isr_alignment_check(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Alignment check fault\n");
    pic_send_eoi(0);
}

void isr_machine_check(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Machine check fault\n");
    asm_cli();
    for(;;) { asm_hlt(); }
}

void isr_simd_exception(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "SIMD exception\n");
    pic_send_eoi(0);
}

void isr_virtualization_exception(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Virtualization exception\n");
    pic_send_eoi(0);
}

/* IRQ handlers */
void isr_timer(struct InterruptFrame* frame) {
    (void)frame;
    pit_handler();
}

void isr_keyboard(struct InterruptFrame* frame) {
    (void)frame;
    keyboard_handler();
}

void isr_cascade(struct InterruptFrame* frame) {
    (void)frame;
    /* No action needed - this is for IRQ2 which cascades IRQ8-15 */
    pic_send_eoi(2);
}

void isr_com2(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "COM2 interrupt\n");
    pic_send_eoi(3);
}

void isr_com1(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "COM1 interrupt\n");
    pic_send_eoi(4);
}

void isr_lpt2(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "LPT2 interrupt\n");
    pic_send_eoi(5);
}

void isr_floppy(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Floppy interrupt\n");
    pic_send_eoi(6);
}

void isr_lpt1(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "LPT1 interrupt\n");
    pic_send_eoi(7);
}

void isr_rtc(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "RTC interrupt\n");
    pic_send_eoi(8);
}

void isr_irq9(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "IRQ9 interrupt\n");
    pic_send_eoi(9);
}

void isr_irq10(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "IRQ10 interrupt\n");
    pic_send_eoi(10);
}

void isr_irq11(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "IRQ11 interrupt\n");
    pic_send_eoi(11);
}

void isr_mouse(struct InterruptFrame* frame) {
    (void)frame;
    /* Forward to mouse driver handler */
    mouse_handler();  /* Mouse handler will send EOI */
    serial_write_string(COM1_PORT, "[ISR] Mouse interrupt handled\n");
}

void isr_coproc(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Coprocessor interrupt\n");
    pic_send_eoi(13);
}

void isr_primary_ata(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Primary ATA interrupt\n");
    pic_send_eoi(14);
}

void isr_secondary_ata(struct InterruptFrame* frame) {
    (void)frame;
    serial_write_string(COM1_PORT, "Secondary ATA interrupt\n");
    pic_send_eoi(15);
} 