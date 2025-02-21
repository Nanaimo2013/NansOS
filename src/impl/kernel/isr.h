/**
 * Interrupt Service Routines
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once

#include "idt.h"

/* Interrupt handlers */
void isr_page_fault_handler(struct InterruptFrame* frame);
void isr_timer_handler(struct InterruptFrame* frame);
void isr_keyboard_handler(struct InterruptFrame* frame); 