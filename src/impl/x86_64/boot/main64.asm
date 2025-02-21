;------------------------------------------------------------------------------
; NansOS 64-bit Entry Point
; Copyright (c) 2025 NansStudios
;
; This file contains the 64-bit entry point for NansOS. It sets up the initial
; environment in long mode and transfers control to the C kernel.
;------------------------------------------------------------------------------

global long_mode_start
extern kernel_main

section .text
bits 64
long_mode_start:
    ; Clear all segment registers for clean 64-bit state
    mov ax, 0
    mov ss, ax  ; Stack segment
    mov ds, ax  ; Data segment
    mov es, ax  ; Extra segment
    mov fs, ax  ; General purpose segment
    mov gs, ax  ; General purpose segment

    ; Pass multiboot info to kernel
    ; rdi and rsi already contain the multiboot info from 32-bit code
    ; No need to modify them as they're already in the correct registers
    ; for the System V AMD64 ABI calling convention

    ; Transfer control to the kernel
    call kernel_main

    ; If kernel returns, halt the CPU
    cli          ; Disable interrupts
    hlt          ; Halt the CPU
.halt:
    jmp .halt    ; Infinite loop if hlt is interrupted
