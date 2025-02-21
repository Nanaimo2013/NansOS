;------------------------------------------------------------------------------
; NansOS Multiboot2 Header
; Copyright (c) 2025 NansStudios
; 
; This file contains the multiboot2 header required for GRUB to recognize
; and load our kernel. The header follows the Multiboot2 Specification.
;------------------------------------------------------------------------------

extern _start

section .multiboot_header    ; Multiboot2 header section
align 8

header_start:
    dd 0xe85250d6           ; Multiboot2 magic number
    dd 0                    ; Architecture: protected mode i386
    dd header_end - header_start    ; Header length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))    ; Checksum

    ; Entry address tag
    align 8
    dw 3                    ; Type: entry address tag
    dw 0                    ; Flags
    dd 12                   ; Size
    dd _start              ; Entry address

    ; Required end tag
    align 8
    dw 0                    ; Type: end tag
    dw 0                    ; Flags
    dd 8                    ; Size

header_end:

; BSS section for end address calculations
section .bss
align 4096
bss_start:
    resb 4096 * 64         ; Reserve 256KB for BSS
bss_end:

load_end:
    ; This label is used to calculate the load end address
