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
    
    ; Exception messages
    exc_msg:         db 'Exception: ', 0
    exc_divzero:     db 'Divide by Zero', 0
    exc_debug:       db 'Debug', 0
    exc_nmi:         db 'Non-Maskable Interrupt', 0
    exc_breakpoint:  db 'Breakpoint', 0
    exc_overflow:    db 'Overflow', 0
    exc_bound:       db 'Bound Range Exceeded', 0
    exc_invalid:     db 'Invalid Opcode', 0
    exc_device:      db 'Device Not Available', 0
    exc_double:      db 'Double Fault', 0
    exc_coprocessor: db 'Coprocessor Segment Overrun', 0
    exc_tss:         db 'Invalid TSS', 0
    exc_segment:     db 'Segment Not Present', 0
    exc_stack:       db 'Stack Segment Fault', 0
    exc_protection:  db 'General Protection Fault', 0
    exc_page:        db 'Page Fault', 0
    exc_fpu:         db 'x87 FPU Error', 0
    exc_alignment:   db 'Alignment Check', 0
    exc_machine:     db 'Machine Check', 0
    exc_simd:        db 'SIMD Floating-Point', 0
    exc_virt:        db 'Virtualization', 0
    exc_security:    db 'Security Exception', 0
    exc_unknown:     db 'Unknown Exception', 0

section .bss
    command_buffer:  resb 256
    memory_size:     resq 1
    cpu_type:        resb 1
    boot_drive:      resb 1
    cursor_pos:      resd 1

section .text
global kernel_main
global exception_handler

kernel_main:
    ; Save registers
    push ebp
    mov ebp, esp
    
    ; Save system info passed from bootloader
    mov [memory_size], eax   ; Memory map address
    mov [cpu_type], bl      ; Memory map entries
    mov [boot_drive], dl    ; Boot drive
    
    ; Print welcome message
    mov esi, welcome_msg
    call print_string
    
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
    mov al, [boot_drive]
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

; Exception handler
; Stack frame:
;   [esp + 40] = Error code (if applicable)
;   [esp + 36] = Exception number
;   [esp + 32] = EIP
;   [esp + 28] = CS
;   [esp + 24] = EFLAGS
exception_handler:
    push ebp
    mov ebp, esp
    
    ; Print exception message
    mov esi, exc_msg
    call print_string
    
    ; Get exception number and lookup message
    mov eax, [ebp + 8]      ; Exception number from stack
    mov esi, exc_unknown    ; Default to unknown
    
    cmp eax, 0
    je .divzero
    cmp eax, 1
    je .debug
    cmp eax, 2
    je .nmi
    cmp eax, 3
    je .breakpoint
    cmp eax, 4
    je .overflow
    cmp eax, 5
    je .bound
    cmp eax, 6
    je .invalid
    cmp eax, 7
    je .device
    cmp eax, 8
    je .double
    cmp eax, 9
    je .coprocessor
    cmp eax, 10
    je .tss
    cmp eax, 11
    je .segment
    cmp eax, 12
    je .stack
    cmp eax, 13
    je .protection
    cmp eax, 14
    je .page
    cmp eax, 16
    je .fpu
    cmp eax, 17
    je .alignment
    cmp eax, 18
    je .machine
    cmp eax, 19
    je .simd
    cmp eax, 20
    je .virt
    jmp .print_msg

.divzero:
    mov esi, exc_divzero
    jmp .print_msg
.debug:
    mov esi, exc_debug
    jmp .print_msg
.nmi:
    mov esi, exc_nmi
    jmp .print_msg
.breakpoint:
    mov esi, exc_breakpoint
    jmp .print_msg
.overflow:
    mov esi, exc_overflow
    jmp .print_msg
.bound:
    mov esi, exc_bound
    jmp .print_msg
.invalid:
    mov esi, exc_invalid
    jmp .print_msg
.device:
    mov esi, exc_device
    jmp .print_msg
.double:
    mov esi, exc_double
    jmp .print_msg
.coprocessor:
    mov esi, exc_coprocessor
    jmp .print_msg
.tss:
    mov esi, exc_tss
    jmp .print_msg
.segment:
    mov esi, exc_segment
    jmp .print_msg
.stack:
    mov esi, exc_stack
    jmp .print_msg
.protection:
    mov esi, exc_protection
    jmp .print_msg
.page:
    mov esi, exc_page
    jmp .print_msg
.fpu:
    mov esi, exc_fpu
    jmp .print_msg
.alignment:
    mov esi, exc_alignment
    jmp .print_msg
.machine:
    mov esi, exc_machine
    jmp .print_msg
.simd:
    mov esi, exc_simd
    jmp .print_msg
.virt:
    mov esi, exc_virt
    jmp .print_msg

.print_msg:
    call print_string
    mov al, 13
    call print_char
    mov al, 10
    call print_char
    
    ; Halt the system
    cli
    hlt

command_loop:
    ; Print prompt
    mov esi, prompt
    call print_string
    
    ; Wait for command
    jmp command_loop

; Print string (ESI = string pointer)
print_string:
    pushad
    
.loop:
    lodsb               ; Load next character
    test al, al        ; Check for null terminator
    jz .done
    call print_char
    jmp .loop
    
.done:
    popad
    ret

; Print character (AL = character)
print_char:
    pushad
    mov edi, 0xB8000    ; Video memory address
    add edi, [cursor_pos]
    mov ah, 0x0A        ; Light green on black
    mov [edi], ax       ; Store character and attribute
    add dword [cursor_pos], 2
    popad
    ret

; Print number in decimal (EAX = number)
print_number:
    pushad
    
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
    
    popad
    ret

section .data
number_buffer: times 32 db 0