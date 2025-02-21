/**
 * Programmable Interrupt Controller (PIC) Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "pic.h"
#include "../port_io/port.h"

/* Initialize PIC */
void pic_init(void) {
    /* Save masks */
    uint8_t mask1 = port_byte_in(PIC1_DATA);
    uint8_t mask2 = port_byte_in(PIC2_DATA);

    /* Start initialization sequence */
    port_byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_io_wait();
    port_byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_io_wait();

    /* Set vector offsets */
    port_byte_out(PIC1_DATA, 0x20);    /* Master PIC vector offset (IRQ 0-7: 0x20-0x27) */
    port_io_wait();
    port_byte_out(PIC2_DATA, 0x28);    /* Slave PIC vector offset (IRQ 8-15: 0x28-0x2F) */
    port_io_wait();

    /* Tell Master PIC that there is a slave PIC at IRQ2 */
    port_byte_out(PIC1_DATA, 4);
    port_io_wait();
    /* Tell Slave PIC its cascade identity */
    port_byte_out(PIC2_DATA, 2);
    port_io_wait();

    /* Set 8086 mode */
    port_byte_out(PIC1_DATA, ICW4_8086);
    port_io_wait();
    port_byte_out(PIC2_DATA, ICW4_8086);
    port_io_wait();

    /* Restore masks - enable timer, keyboard, and cascade */
    mask1 = 0xF8;  /* 1111 1000 - Enable IRQ0 (timer), IRQ1 (keyboard), IRQ2 (cascade) */
    mask2 = 0xFF;  /* All disabled on slave PIC initially */
    
    port_byte_out(PIC1_DATA, mask1);
    port_io_wait();
    port_byte_out(PIC2_DATA, mask2);
    port_io_wait();
}

/* Send end of interrupt signal */
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }
    port_byte_out(PIC1_COMMAND, PIC_EOI);
}

/* Set interrupt mask */
void pic_set_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = port_byte_in(port) | (1 << irq);
    port_byte_out(port, value);
}

/* Clear interrupt mask */
void pic_clear_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = port_byte_in(port) & ~(1 << irq);
    port_byte_out(port, value);
}

/* Disable PIC */
void pic_disable(void) {
    /* Mask all interrupts */
    port_byte_out(PIC1_DATA, 0xFF);
    port_byte_out(PIC2_DATA, 0xFF);
} 