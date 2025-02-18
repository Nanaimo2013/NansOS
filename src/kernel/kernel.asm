[BITS 32]
; Main kernel code for NansOS

extern display_init
extern kbd_init
extern kbd_read
extern mouse_init
extern fs_init
extern desktop_init

section .data
    welcome_msg:     db 'NansOS 32-bit Kernel', 13, 10
                    db 'Copyright (c) 2024 NanCo Industries', 13, 10, 0
    init_msg:        db 'Initializing system components...', 13, 10, 0
    display_msg:     db 'Initializing display...', 13, 10, 0
    keyboard_msg:    db 'Initializing keyboard...', 13, 10, 0
    mouse_msg:       db 'Initializing mouse...', 13, 10, 0
    filesystem_msg:  db 'Initializing filesystem...', 13, 10, 0
    memory_msg:      db 'System Memory: ', 0
    kb_msg:         db ' KB', 13, 10, 0
    cpu_msg:        db 'CPU Type: ', 0
    cpu_types:      db '8086/8088', 0
                    db '80286   ', 0
                    db '80386   ', 0
                    db '80486   ', 0
    gui_msg:         db 'Starting GUI...', 13, 10, 0
    done_msg:        db 'System initialization complete.', 13, 10, 0
    prompt:          db '> ', 0
    current_pos:     dq 0

section .bss
    command_buffer:  resb 256
    memory_size:     resq 1
    cpu_type:        resb 1
    bootDrive:       resb 1

section .text
global kernel_main

kernel_main:
    ; Save registers
    push ebp
    mov ebp, esp
    
    ; Save system info passed from bootloader
    mov [memory_size], eax   ; Memory size
    mov [cpu_type], bl      ; CPU type
    mov [bootDrive], dl     ; Boot drive
    
    ; Print welcome message
    mov esi, welcome_msg
    call print_string
    
    ; Initialize video mode
    mov ax, 0x0003  ; Text mode 80x25
    int 0x10
    
    ; Print initialization message
    mov esi, init_msg
    call print_string
    
    ; Display CPU information
    mov esi, cpu_msg
    call print_string
    movzx eax, byte [cpu_type]
    shl eax, 3              ; Multiply by 8 (length of each CPU type string)
    lea esi, [cpu_types + eax]
    call print_string
    
    ; Display memory information
    mov esi, memory_msg
    call print_string
    mov eax, [memory_size]
    call print_number
    mov esi, kb_msg
    call print_string
    
    ; Initialize display
    mov esi, display_msg
    call print_string
    call display_init
    
    ; Initialize keyboard
    mov esi, keyboard_msg
    call print_string
    call kbd_init
    
    ; Initialize mouse
    mov esi, mouse_msg
    call print_string
    call mouse_init
    
    ; Initialize filesystem
    mov esi, filesystem_msg
    call print_string
    mov al, [bootDrive]
    push eax
    call fs_init
    add esp, 4
    
    ; Initialize GUI
    mov esi, gui_msg
    call print_string
    call desktop_init
    
    ; Display completion message
    mov esi, done_msg
    call print_string
    
    ; Enter command loop
    jmp command_loop

command_loop:
    ; Print prompt
    mov esi, prompt
    call print_string
    
    ; Wait for command
    jmp command_loop

; Print string (ESI = string pointer)
print_string:
    push eax
    push ebx
    push ecx
    push edx
    
    mov ebx, 0xB8000    ; Video memory address
    mov ah, 0x0A        ; Light green on black
    
.loop:
    lodsb               ; Load next character
    test al, al        ; Check for null terminator
    jz .done
    
    mov [ebx], al      ; Store character
    mov [ebx+1], ah    ; Store attribute
    add ebx, 2         ; Next character position
    
    jmp .loop
.done:
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; Print number in decimal
; Input: EAX = number to print
print_number:
    push ebx
    push ecx
    push edx
    push edi
    
    mov ebx, number_buffer + 31  ; End of buffer
    mov byte [ebx], 0            ; Null terminator
    mov ecx, 10                  ; Divisor
    
    test eax, eax
    jnz .convert
    mov byte [ebx-1], '0'        ; Special case for zero
    dec ebx
    jmp .print
    
.convert:
    test eax, eax
    jz .print
    
    xor edx, edx
    div ecx                     ; Divide by 10
    add dl, '0'                 ; Convert remainder to ASCII
    dec ebx
    mov [ebx], dl              ; Store digit
    jmp .convert
    
.print:
    mov esi, ebx
    call print_string
    
    pop edi
    pop edx
    pop ecx
    pop ebx
    ret

; Get command from keyboard
get_command:
    push eax
    push ebx
    push ecx
    push edx
    xor ecx, ecx       ; Character counter
.loop:
    ; Wait for key
    call kbd_read
    test al, al
    jz .loop
    
    ; Check for enter key
    cmp al, 13
    je .done
    
    ; Check for backspace
    cmp al, 8
    je .backspace
    
    ; Store character if buffer not full
    cmp ecx, 254
    jae .loop
    
    ; Echo character
    push eax
    call print_char
    pop eax
    
    ; Store character
    stosb
    inc ecx
    jmp .loop

.backspace:
    test ecx, ecx
    jz .loop
    
    ; Move cursor back
    mov al, 8
    call print_char
    mov al, ' '
    call print_char
    mov al, 8
    call print_char
    
    ; Update buffer
    dec edi
    dec ecx
    jmp .loop

.done:
    ; Add newline
    mov al, 13
    call print_char
    mov al, 10
    call print_char
    
    ; Null terminate command
    mov byte [edi], 0
    
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; Process command
process_command:
    push eax
    push ebx
    push ecx
    push edx
    
    ; Compare with known commands
    ; TODO: Implement command processing
    
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; Print character
print_char:
    push ebx
    push ecx
    
    mov ah, 0x0A        ; Light green on black
    mov ebx, [rel current_pos]
    shl ebx, 1
    add ebx, 0xB8000
    mov [ebx], ax
    
    inc dword [rel current_pos]
    
    pop ecx
    pop ebx
    ret

section .data
number_buffer: times 32 db 0