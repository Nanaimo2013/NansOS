[BITS 32]
; Kernel Entry Point for NansOS

section .text
global _start

_start:
    ; Set up segment registers
    mov ax, 0x10       ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000   ; Set up stack

    ; Initialize kernel
    call init_kernel

    ; Enter command loop
    jmp command_loop

init_kernel:
    ; Clear screen
    mov edi, 0xB8000    ; Video memory address
    mov ecx, 2000       ; 80x25 characters (one word each)
    mov ax, 0x0720      ; Light gray space on black background
    rep stosw

    ; Print welcome message
    mov esi, welcome_msg
    call print_string
    
    ; Print version info
    mov esi, version_msg
    call print_string
    
    ; Print status message
    mov esi, status_msg
    call print_string
    
    ; Print prompt
    mov esi, prompt
    call print_string
    ret

command_loop:
    ; Wait for keyboard input
    mov esi, prompt
    call print_string
    jmp command_loop

; Print string (ESI = string pointer)
print_string:
    push eax
    push ecx
    push edi
    
    mov edi, [cursor_pos]  ; Get current cursor position
    shl edi, 1             ; Multiply by 2 (each character is 2 bytes)
    add edi, 0xB8000       ; Add video memory base
    mov ah, 0x0A           ; Light green on black
    
.loop:
    lodsb                  ; Load next character
    test al, al            ; Check for null terminator
    jz .done
    
    cmp al, 10            ; Check for newline
    je .newline
    
    mov [edi], al         ; Store character
    mov [edi + 1], ah     ; Store attribute
    add edi, 2            ; Next character position
    inc dword [cursor_pos]
    jmp .loop

.newline:
    mov eax, [cursor_pos]
    mov ecx, 80           ; Screen width
    div ecx               ; Divide by screen width
    inc eax               ; Move to next line
    mul ecx               ; Multiply by screen width
    mov [cursor_pos], eax ; Update cursor position
    jmp .loop
    
.done:
    pop edi
    pop ecx
    pop eax
    ret

section .data
welcome_msg:    db 'NansOS Kernel v1.0.0', 10, 0
version_msg:    db 'Copyright (c) 2024 NanCo Industries', 10, 0
status_msg:     db 'System initialized in protected mode', 10, 0
prompt:         db '> ', 0

section .bss
cursor_pos:     resd 1    ; Current cursor position (in characters) 