; Multiboot header constants
MULTIBOOT_MAGIC    equ 0x1BADB002
MULTIBOOT_FLAGS    equ 0x00000003  ; Align modules and provide memory info
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:

section .text
global _start
extern kernel_main

; Exception handlers
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0    ; Dummy error code
    push dword %1   ; Interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1   ; Interrupt number
    jmp isr_common
%endmacro

ISR_NOERRCODE 0    ; Divide by zero
ISR_NOERRCODE 1    ; Debug
ISR_NOERRCODE 2    ; Non-maskable interrupt
ISR_NOERRCODE 3    ; Breakpoint
ISR_NOERRCODE 4    ; Overflow
ISR_NOERRCODE 5    ; Bound range exceeded
ISR_NOERRCODE 6    ; Invalid opcode
ISR_NOERRCODE 7    ; Device not available
ISR_ERRCODE   8    ; Double fault
ISR_NOERRCODE 9    ; Coprocessor segment overrun
ISR_ERRCODE   10   ; Invalid TSS
ISR_ERRCODE   11   ; Segment not present
ISR_ERRCODE   12   ; Stack segment fault
ISR_ERRCODE   13   ; General protection fault
ISR_ERRCODE   14   ; Page fault
ISR_NOERRCODE 15   ; Reserved
ISR_NOERRCODE 16   ; x87 FPU error
ISR_ERRCODE   17   ; Alignment check
ISR_NOERRCODE 18   ; Machine check
ISR_NOERRCODE 19   ; SIMD floating-point
ISR_NOERRCODE 20   ; Virtualization
ISR_NOERRCODE 21   ; Reserved
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Reserved
ISR_NOERRCODE 29   ; Reserved
ISR_NOERRCODE 30   ; Reserved
ISR_NOERRCODE 31   ; Reserved

; Common exception handler
isr_common:
    pushad          ; Save registers
    
    mov ax, ds      ; Save data segment
    push eax
    
    mov ax, 0x10    ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp        ; Push pointer to registers
    
    ; Call C handler
    extern exception_handler
    call exception_handler
    
    add esp, 4      ; Clean up stack
    
    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popad           ; Restore registers
    add esp, 8      ; Clean up error code and interrupt number
    
    iret            ; Return from interrupt

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot info
    push ebx    ; Multiboot info structure pointer
    push eax    ; Should be 0x2BADB002 (multiboot magic)
    
    ; Initialize essential CPU features
    call init_cpu
    
    ; Initialize basic paging
    call setup_paging
    
    ; Initialize interrupt descriptor table
    call setup_idt
    
    ; Initialize PIC
    call init_pic
    
    ; Initialize system timer
    call init_timer
    
    ; Clear screen
    call clear_screen
    
    ; Print welcome message
    mov esi, msg_welcome
    call print_string
    
    ; Enable interrupts
    sti
    
    ; Restore multiboot parameters and call kernel_main
    pop eax     ; Multiboot magic
    pop ebx     ; Multiboot info structure
    push ebx    ; Pass as parameter to kernel_main
    call kernel_main
    
    ; Should never return
    cli
    hlt

; Initialize CPU features
init_cpu:
    ; Clear direction flag
    cld
    
    ; Initialize FPU
    fninit
    ret

; Set up basic paging
setup_paging:
    pushad
    
    ; Clear page directory
    mov edi, PAGE_DIR
    xor eax, eax
    mov ecx, 1024
    rep stosd
    
    ; Identity map first 4MB
    mov edi, PAGE_TABLE
    mov eax, 0x00000003    ; Present + R/W
    mov ecx, 1024
.map_page:
    stosd
    add eax, 0x1000
    loop .map_page
    
    ; Set up page directory entry
    mov eax, PAGE_TABLE
    or eax, 3              ; Present + R/W
    mov [PAGE_DIR], eax
    
    ; Load page directory
    mov eax, PAGE_DIR
    mov cr3, eax
    
    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    popad
    ret

; Initialize PIC
init_pic:
    ; Initialize master PIC
    mov al, 0x11        ; ICW1: Initialize + ICW4
    out 0x20, al
    mov al, 0x20        ; ICW2: IRQ 0-7 -> INT 0x20-0x27
    out 0x21, al
    mov al, 0x04        ; ICW3: IRQ2 -> Slave
    out 0x21, al
    mov al, 0x01        ; ICW4: 8086 mode
    out 0x21, al
    
    ; Initialize slave PIC
    mov al, 0x11        ; ICW1
    out 0xA0, al
    mov al, 0x28        ; ICW2: IRQ 8-15 -> INT 0x28-0x2F
    out 0xA1, al
    mov al, 0x02        ; ICW3: Slave ID 2
    out 0xA1, al
    mov al, 0x01        ; ICW4
    out 0xA1, al
    
    ; Mask all interrupts except keyboard and timer
    mov al, 0xFC        ; Enable IRQ0 (timer) and IRQ1 (keyboard)
    out 0x21, al
    mov al, 0xFF        ; Mask all slave interrupts
    out 0xA1, al
    ret

; Initialize system timer
init_timer:
    ; Set timer frequency to 100 Hz
    mov al, 0x36        ; Channel 0, LSB/MSB, Mode 3
    out 0x43, al
    mov al, 0x9C        ; LSB: 11932 = 1193182 / 100
    out 0x40, al
    mov al, 0x2E        ; MSB
    out 0x40, al
    ret

; Set up Interrupt Descriptor Table
setup_idt:
    pushad
    
    ; Clear IDT
    mov edi, IDT_BASE
    xor eax, eax
    mov ecx, 256 * 8    ; 256 entries * 8 bytes
    rep stosb
    
    ; Set up exception handlers
    mov edi, IDT_BASE
    mov esi, isr_table
    mov ecx, 32         ; 32 exception handlers
    
.setup_handler:
    push ecx
    
    mov eax, [esi]      ; Get handler address
    mov word [edi], ax  ; Handler low bits
    shr eax, 16
    mov word [edi + 6], ax  ; Handler high bits
    
    mov word [edi + 2], 0x08  ; Code segment selector
    mov byte [edi + 4], 0x00  ; Reserved
    mov byte [edi + 5], 0x8E  ; Present, Ring 0, 32-bit Interrupt Gate
    
    add edi, 8          ; Next IDT entry
    add esi, 4          ; Next handler address
    
    pop ecx
    loop .setup_handler
    
    ; Load IDT
    lidt [IDT_DESC]
    
    popad
    ret

; Clear screen
clear_screen:
    pushad
    mov edi, 0xB8000    ; Video memory
    mov ecx, 2000       ; 80x25x2 (characters + attributes)
    mov ax, 0x0720      ; White on black space
    rep stosw
    mov dword [cursor_pos], 0
    popad
    ret

; Print string
; ESI = string pointer
print_string:
    pushad
.loop:
    lodsb
    test al, al
    jz .done
    call print_char
    jmp .loop
.done:
    popad
    ret

; Print character
; AL = character
print_char:
    pushad
    mov edi, 0xB8000
    add edi, [cursor_pos]
    mov ah, 0x0A        ; Light green on black
    mov [edi], ax
    add dword [cursor_pos], 2
    
    ; Handle newline
    cmp al, 13          ; CR
    jne .check_lf
    mov eax, [cursor_pos]
    mov ecx, 160        ; Line width in bytes
    xor edx, edx
    div ecx
    mul ecx
    mov [cursor_pos], eax
    jmp .check_overflow
    
.check_lf:
    cmp al, 10          ; LF
    jne .check_overflow
    add dword [cursor_pos], 160
    
.check_overflow:
    ; Check for screen overflow
    cmp dword [cursor_pos], 4000
    jb .done
    
    ; Scroll screen
    pushad
    mov esi, 0xB8000 + 160  ; Second line
    mov edi, 0xB8000        ; First line
    mov ecx, 1920           ; 24 lines * 80 chars
    rep movsw
    
    ; Clear bottom line
    mov edi, 0xB8000 + 3840 ; Last line
    mov ecx, 80             ; 80 chars
    mov ax, 0x0720          ; White on black space
    rep stosw
    
    ; Adjust cursor
    sub dword [cursor_pos], 160
    popad
    
.done:
    popad
    ret

section .data
    align 4096
    PAGE_DIR:       times 1024 dd 0    ; Page directory
    PAGE_TABLE:     times 1024 dd 0    ; First page table
    
    align 8
    IDT_DESC:
        dw 256 * 8 - 1                 ; IDT size
        dd IDT_BASE                    ; IDT address
    
    ; Table of exception handler addresses
    isr_table:
        dd isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
        dd isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
        dd isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
        dd isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
    
    cursor_pos:      dd 0
    
    msg_welcome:     db 'NansOS Kernel v1.0 (GRUB)', 13, 10
                    db 'Copyright (c) 2024 NanCo Industries', 13, 10, 0

section .bss
    align 4096
    IDT_BASE:       resb 256 * 8    ; Space for IDT