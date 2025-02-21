/**
 * Programmable Interrupt Controller (PIC) Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* PIC ports */
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

/* PIC commands */
#define PIC_EOI         0x20    /* End of interrupt command */
#define ICW1_ICW4       0x01    /* ICW4 needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 */
#define ICW1_LEVEL      0x08    /* Level triggered mode */
#define ICW1_INIT       0x10    /* Initialization command */
#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested */

/* Function declarations */
void pic_init(void);                    /* Initialize PIC */
void pic_send_eoi(uint8_t irq);         /* Send end of interrupt */
void pic_disable(void);                 /* Disable PIC */
void pic_set_mask(uint8_t irq);         /* Set interrupt mask */
void pic_clear_mask(uint8_t irq);       /* Clear interrupt mask */ 