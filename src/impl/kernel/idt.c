/**
 * Interrupt Descriptor Table Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "idt.h"
#include "isr.h"
#include "../drivers/serial/serial.h"
#include <stdio.h>

/* IDT entries */
static struct IDTEntry idt[256];
static struct IDTPointer idt_pointer;

/* Initialize IDT entry */
void idt_set_entry(uint8_t vector, uint64_t isr, uint8_t flags) {
    struct IDTEntry* entry = &idt[vector];
    
    entry->isr_low = isr & 0xFFFF;
    entry->kernel_cs = 0x08;  /* Kernel code segment */
    entry->ist = 0;          /* No interrupt stack table */
    entry->attributes = flags;
    entry->isr_mid = (isr >> 16) & 0xFFFF;
    entry->isr_high = (isr >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;
}

/* Initialize IDT */
void idt_init(void) {
    /* Clear IDT */
    for (int i = 0; i < 256; i++) {
        idt_set_entry(i, (uint64_t)isr_default_handler, 0x8E);  /* Present, Ring 0, Interrupt Gate */
    }
    
    /* Set up exception handlers */
    idt_set_entry(0, (uint64_t)isr_divide_by_zero, 0x8E);
    idt_set_entry(1, (uint64_t)isr_debug, 0x8E);
    idt_set_entry(2, (uint64_t)isr_nmi, 0x8E);
    idt_set_entry(3, (uint64_t)isr_breakpoint, 0x8E);
    idt_set_entry(4, (uint64_t)isr_overflow, 0x8E);
    idt_set_entry(5, (uint64_t)isr_bound_range, 0x8E);
    idt_set_entry(6, (uint64_t)isr_invalid_opcode, 0x8E);
    idt_set_entry(7, (uint64_t)isr_device_not_available, 0x8E);
    idt_set_entry(8, (uint64_t)isr_double_fault, 0x8E);
    idt_set_entry(10, (uint64_t)isr_invalid_tss, 0x8E);
    idt_set_entry(11, (uint64_t)isr_segment_not_present, 0x8E);
    idt_set_entry(12, (uint64_t)isr_stack_segment, 0x8E);
    idt_set_entry(13, (uint64_t)isr_general_protection, 0x8E);
    idt_set_entry(14, (uint64_t)isr_page_fault_handler, 0x8E);
    idt_set_entry(16, (uint64_t)isr_fpu_fault, 0x8E);
    idt_set_entry(17, (uint64_t)isr_alignment_check, 0x8E);
    idt_set_entry(18, (uint64_t)isr_machine_check, 0x8E);
    idt_set_entry(19, (uint64_t)isr_simd_exception, 0x8E);
    idt_set_entry(20, (uint64_t)isr_virtualization_exception, 0x8E);
    
    /* Set up IRQ handlers */
    idt_set_entry(32, (uint64_t)isr_timer, 0x8E);          /* IRQ 0 - Timer */
    idt_set_entry(33, (uint64_t)isr_keyboard, 0x8E);       /* IRQ 1 - Keyboard */
    idt_set_entry(34, (uint64_t)isr_cascade, 0x8E);        /* IRQ 2 - Cascade for IRQ 8-15 */
    idt_set_entry(35, (uint64_t)isr_com2, 0x8E);          /* IRQ 3 - COM2 */
    idt_set_entry(36, (uint64_t)isr_com1, 0x8E);          /* IRQ 4 - COM1 */
    idt_set_entry(37, (uint64_t)isr_lpt2, 0x8E);          /* IRQ 5 - LPT2 */
    idt_set_entry(38, (uint64_t)isr_floppy, 0x8E);        /* IRQ 6 - Floppy */
    idt_set_entry(39, (uint64_t)isr_lpt1, 0x8E);          /* IRQ 7 - LPT1 */
    idt_set_entry(40, (uint64_t)isr_rtc, 0x8E);           /* IRQ 8 - RTC */
    idt_set_entry(41, (uint64_t)isr_irq9, 0x8E);          /* IRQ 9 */
    idt_set_entry(42, (uint64_t)isr_irq10, 0x8E);         /* IRQ 10 */
    idt_set_entry(43, (uint64_t)isr_irq11, 0x8E);         /* IRQ 11 */
    idt_set_entry(44, (uint64_t)isr_mouse, 0x8E);         /* IRQ 12 - Mouse */
    idt_set_entry(45, (uint64_t)isr_coproc, 0x8E);        /* IRQ 13 - Coprocessor */
    idt_set_entry(46, (uint64_t)isr_primary_ata, 0x8E);   /* IRQ 14 - Primary ATA */
    idt_set_entry(47, (uint64_t)isr_secondary_ata, 0x8E); /* IRQ 15 - Secondary ATA */
    
    /* Set up IDT pointer */
    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.base = (uint64_t)&idt;
    
    /* Load IDT */
    asm volatile("lidt %0" : : "m"(idt_pointer));
    
    /* Send initialization message to serial */
    serial_write_string(COM1_PORT, "IDT initialized\n");
}

/* Enable interrupts */
void idt_enable_interrupts(void) {
    asm volatile("sti");
    serial_write_string(COM1_PORT, "Interrupts enabled\n");
}

/* Disable interrupts */
void idt_disable_interrupts(void) {
    asm volatile("cli");
    serial_write_string(COM1_PORT, "Interrupts disabled\n");
}