/**
 * Port I/O Operations
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>
#include "../../../impl/kernel/asm_utils.h"

/* Port I/O Functions */
uint8_t port_byte_in(uint16_t port);
uint16_t port_word_in(uint16_t port);
void port_byte_out(uint16_t port, uint8_t data);
void port_word_out(uint16_t port, uint16_t data);
void port_dword_out(uint16_t port, uint32_t value);
uint32_t port_dword_in(uint16_t port);
void port_io_wait(void);  /* Add small delay between I/O operations */ 