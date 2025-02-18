[BITS 64]
; 64-bit Kernel Main for NansOS

extern display_init
extern kbd_init
extern mouse_init
extern fs_init

section .data
    welcome_msg:     db 'NansOS 64-bit Kernel', 13, 10
                    db 'Copyright (c) 2024 NanCo Industries', 13, 10, 0
    init_msg:        db 'Initializing system components...', 13, 10, 0
    display_msg:     db 'Initializing display...', 13, 10, 0
    keyboard_msg:    db 'Initializing keyboard...', 13, 10, 0
    mouse_msg:       db 'Initializing mouse...', 13, 10, 0
    filesystem_msg:  db 'Initializing filesystem...', 13, 10, 0
    memory_msg:      db 'Detecting system memory...', 13, 10, 0
    cpu_msg:         db 'Detecting CPU features...', 13, 10, 0
    done_msg:        db 'System initialization complete.', 13, 10, 0
    prompt:          db '> ', 0

section .bss
    command_buffer:  resb 256
    memory_size:     resq 1
    cpu_vendor:      resb 13
    cpu_brand:       resb 49
    bootDrive:       resb 1

section .text
global kernel_main

kernel_main:
    ; Save registers
    push rbp
    mov rbp, rsp
    
    ; Display welcome message
    mov rsi, welcome_msg
    call print_string
    
    ; Initialize system components
    mov rsi, init_msg
    call print_string
    
    ; Detect CPU information
    mov rsi, cpu_msg
    call print_string
    call detect_cpu
    
    ; Detect memory
    mov rsi, memory_msg
    call print_string
    call detect_memory
    
    ; Initialize display
    mov rsi, display_msg
    call print_string
    call display_init
    
    ; Initialize keyboard
    mov rsi, keyboard_msg
    call print_string
    call kbd_init
    
    ; Initialize mouse
    mov rsi, mouse_msg
    call print_string
    call mouse_init
    
    ; Initialize filesystem
    mov rsi, filesystem_msg
    call print_string
    mov al, [bootDrive]
    push rax
    call fs_init
    add rsp, 8
    
    ; Display completion message
    mov rsi, done_msg
    call print_string
    
    ; Enter command loop
    jmp command_loop

command_loop:
    ; Display prompt
    mov rsi, prompt
    call print_string
    
    ; Get command
    mov rdi, command_buffer
    call get_command
    
    ; Process command
    mov rsi, command_buffer
    call process_command
    
    ; Repeat
    jmp command_loop

; Print string (null-terminated)
print_string:
    push rax
    push rbx
    push rcx
    push rdx
.loop:
    lodsb               ; Load next character
    test al, al        ; Check for null terminator
    jz .done
    
    ; Use system call to print character
    push rsi
    mov rsi, rsp       ; Point to character on stack
    mov rdx, 1         ; Length = 1 character
    mov rax, 1         ; sys_write
    mov rdi, 1         ; stdout
    syscall
    pop rsi
    
    jmp .loop
.done:
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; Get command from keyboard
get_command:
    push rax
    push rbx
    push rcx
    push rdx
    xor rcx, rcx       ; Character counter
.loop:
    ; Read a single character
    push rcx
    mov rdx, 1         ; Read 1 character
    mov rax, 0         ; sys_read
    mov rdi, 0         ; stdin
    mov rsi, rsp       ; Buffer on stack
    syscall
    pop rax            ; Get character
    
    ; Check for enter key
    cmp al, 13
    je .done
    
    ; Check for backspace
    cmp al, 8
    je .backspace
    
    ; Store character if buffer not full
    cmp rcx, 254
    jae .loop
    
    ; Echo character
    push rax
    mov rdx, 1         ; Length = 1
    mov rax, 1         ; sys_write
    mov rdi, 1         ; stdout
    mov rsi, rsp       ; Character on stack
    syscall
    pop rax
    
    ; Store character
    stosb
    inc rcx
    jmp .loop

.backspace:
    test rcx, rcx
    jz .loop
    
    ; Move cursor back
    push rcx
    mov al, 8
    push rax
    mov rdx, 1
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    syscall
    
    ; Clear character
    mov al, ' '
    mov [rsp], al
    mov rdx, 1
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    syscall
    
    ; Move cursor back again
    mov al, 8
    mov [rsp], al
    mov rdx, 1
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    syscall
    pop rax
    pop rcx
    
    ; Update buffer
    dec rdi
    dec rcx
    jmp .loop

.done:
    ; Add newline
    push rax
    mov al, 13
    mov [rsp], al
    mov rdx, 1
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    syscall
    mov al, 10
    mov [rsp], al
    mov rdx, 1
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    syscall
    pop rax
    
    ; Null terminate command
    mov byte [rdi], 0
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; Process command
process_command:
    push rax
    push rbx
    push rcx
    push rdx
    
    ; Compare with known commands
    ; TODO: Implement command processing
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; Detect CPU features
detect_cpu:
    push rax
    push rbx
    push rcx
    push rdx
    
    ; Get vendor string
    xor rax, rax
    cpuid
    mov [cpu_vendor], ebx
    mov [cpu_vendor + 4], edx
    mov [cpu_vendor + 8], ecx
    mov byte [cpu_vendor + 12], 0
    
    ; Get brand string
    mov rax, 0x80000002
    cpuid
    mov [cpu_brand], eax
    mov [cpu_brand + 4], ebx
    mov [cpu_brand + 8], ecx
    mov [cpu_brand + 12], edx
    
    mov rax, 0x80000003
    cpuid
    mov [cpu_brand + 16], eax
    mov [cpu_brand + 20], ebx
    mov [cpu_brand + 24], ecx
    mov [cpu_brand + 28], edx
    
    mov rax, 0x80000004
    cpuid
    mov [cpu_brand + 32], eax
    mov [cpu_brand + 36], ebx
    mov [cpu_brand + 40], ecx
    mov [cpu_brand + 44], edx
    mov byte [cpu_brand + 48], 0
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; Detect system memory
detect_memory:
    push rax
    push rbx
    push rcx
    push rdx
    
    ; Use int 0x15, eax = 0xE820 for memory detection
    mov rax, 0xE820
    xor rbx, rbx
    mov rcx, 24
    mov rdx, 0x534D4150  ; 'SMAP'
    mov rdi, memory_size
    int 0x15
    
    ; Store total memory size
    mov rax, [rdi + 8]   ; Length of memory region
    mov [memory_size], rax
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret