[BITS 32]
; Kernel Entry Point for NansOS

section .text
global _start

_start:
    ; Set up stack
    mov esp, 0x90000

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
    
    ; Print prompt
    call new_line
    mov esi, prompt
    call print_string

    ; Enter command loop
    jmp command_loop

command_loop:
    ; Get keyboard input
    mov ah, 0           ; Function 0 - get keystroke
    int 0x16           ; BIOS keyboard interrupt
    
    ; Check for Enter key
    cmp al, 13
    je .process_command
    
    ; Check for backspace
    cmp al, 8
    je .handle_backspace
    
    ; Echo character
    mov ah, 0x0A        ; Light green on black
    call print_char
    
    jmp command_loop

.handle_backspace:
    ; Move cursor back
    mov al, ' '
    mov ah, 0x0A
    call print_char
    mov al, 8
    call print_char
    jmp command_loop

.process_command:
    ; Print newline
    call new_line
    
    ; Print next prompt
    mov esi, prompt
    call print_string
    
    jmp command_loop

; Print string (ESI = string pointer)
print_string:
    push eax
    push ebx
    push ecx
    push edx
    
    mov ebx, [cursor_pos]  ; Get current cursor position
    add ebx, 0xB8000       ; Add video memory base
    mov ah, 0x0A           ; Light green on black
    
.loop:
    lodsb                  ; Load next character
    test al, al           ; Check for null terminator
    jz .done
    
    mov [ebx], al         ; Store character
    mov [ebx+1], ah       ; Store attribute
    add ebx, 2            ; Next character position
    
    jmp .loop
.done:
    sub ebx, 0xB8000      ; Calculate new cursor position
    mov [cursor_pos], ebx
    
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; Print character in AL with attribute AH
print_char:
    push ebx
    
    mov ebx, [cursor_pos]
    add ebx, 0xB8000
    mov [ebx], ax        ; Store character and attribute
    add ebx, 2
    sub ebx, 0xB8000
    mov [cursor_pos], ebx
    
    pop ebx
    ret

; Print newline
new_line:
    push eax
    push ebx
    push edx
    
    mov eax, [cursor_pos]
    mov ebx, 160         ; 80 characters * 2 bytes per character
    xor edx, edx
    div ebx
    inc eax
    mul ebx
    mov [cursor_pos], eax
    
    pop edx
    pop ebx
    pop eax
    ret

section .data
align 4
welcome_msg:  db 'NansOS Kernel v1.0 loaded successfully!', 0
version_msg:  db 'Copyright (c) 2024 NanCo Industries', 0
prompt:       db '> ', 0
cursor_pos:   dd 0      ; Current cursor position in video memory 