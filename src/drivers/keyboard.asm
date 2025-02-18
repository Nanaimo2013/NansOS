[BITS 32]
; Keyboard Driver for NansOS
; Supports both PS/2 and USB keyboards

; Keyboard controller ports
KBD_DATA_PORT    equ 0x60
KBD_STATUS_PORT  equ 0x64
KBD_COMMAND_PORT equ 0x64

; Keyboard commands
KBD_LED_CMD      equ 0xED
KBD_ENABLE_CMD   equ 0xF4
KBD_RESET_CMD    equ 0xFF

section .data
    kbd_buffer:      times 128 db 0  ; Circular buffer for keyboard input
    kbd_buffer_head: dd 0            ; Buffer head pointer
    kbd_buffer_tail: dd 0            ; Buffer tail pointer
    kbd_flags:       db 0            ; Keyboard status flags
    kbd_type:        db 0            ; 0 = PS/2, 1 = USB

section .text
global kbd_init
global kbd_handler
global kbd_read
global kbd_flush

kbd_init:
    push ebp
    mov ebp, esp
    
    ; Disable keyboard
    call kbd_wait_write
    mov al, 0xAD
    out KBD_COMMAND_PORT, al
    
    ; Flush output buffer
    call kbd_flush
    
    ; Set keyboard configuration
    call kbd_wait_write
    mov al, 0x20        ; Read command byte
    out KBD_COMMAND_PORT, al
    
    call kbd_wait_read
    in al, KBD_DATA_PORT
    push eax            ; Save configuration
    
    and al, 0x54       ; Clear unwanted bits
    or al, 0x43        ; Set wanted bits
    
    call kbd_wait_write
    mov al, 0x60        ; Write command byte
    out KBD_COMMAND_PORT, al
    
    call kbd_wait_write
    pop eax
    out KBD_DATA_PORT, al
    
    ; Enable keyboard
    call kbd_wait_write
    mov al, 0xAE
    out KBD_COMMAND_PORT, al
    
    ; Reset keyboard
    call kbd_reset
    
    mov esp, ebp
    pop ebp
    ret

kbd_handler:
    push ebp
    mov ebp, esp
    pushad
    
    ; Read scan code
    in al, KBD_DATA_PORT
    
    ; Store in buffer
    mov edi, [kbd_buffer_head]
    mov [kbd_buffer + edi], al
    
    ; Update buffer head
    inc edi
    and edi, 127        ; Wrap around buffer (128 bytes)
    mov [kbd_buffer_head], edi
    
    ; Send EOI to PIC
    mov al, 0x20
    out 0x20, al
    
    popad
    mov esp, ebp
    pop ebp
    iret

kbd_read:
    push ebp
    mov ebp, esp
    
    ; Check if buffer is empty
    mov eax, [kbd_buffer_tail]
    cmp eax, [kbd_buffer_head]
    je .no_data
    
    ; Read from buffer
    mov edi, [kbd_buffer_tail]
    movzx eax, byte [kbd_buffer + edi]
    
    ; Update buffer tail
    inc edi
    and edi, 127        ; Wrap around buffer
    mov [kbd_buffer_tail], edi
    
    jmp .done
    
.no_data:
    xor eax, eax        ; Return 0 if no data
    
.done:
    mov esp, ebp
    pop ebp
    ret

kbd_flush:
    push ebp
    mov ebp, esp
    
.flush_loop:
    ; Check keyboard status
    in al, KBD_STATUS_PORT
    test al, 1          ; Output buffer full?
    jz .done
    
    ; Read and discard data
    in al, KBD_DATA_PORT
    jmp .flush_loop
    
.done:
    mov esp, ebp
    pop ebp
    ret

kbd_wait_read:
    in al, KBD_STATUS_PORT
    test al, 1
    jz kbd_wait_read
    ret

kbd_wait_write:
    in al, KBD_STATUS_PORT
    test al, 2
    jnz kbd_wait_write
    ret

kbd_reset:
    ; Send reset command
    call kbd_wait_write
    mov al, KBD_RESET_CMD
    out KBD_DATA_PORT, al
    
    ; Wait for ACK
    call kbd_wait_read
    in al, KBD_DATA_PORT
    cmp al, 0xFA        ; ACK
    jne kbd_reset       ; Retry if no ACK
    
    ; Wait for self-test completion
    call kbd_wait_read
    in al, KBD_DATA_PORT
    cmp al, 0xAA        ; Self-test passed
    jne kbd_reset       ; Retry if failed
    
    ret 