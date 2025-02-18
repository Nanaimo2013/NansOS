[BITS 64]
; Utility functions for NansOS

section .data
    video_memory    equ 0xB8000
    screen_width    equ 80
    screen_height   equ 25
    cursor_pos      dq 0
    default_attr    db 0x07    ; Light gray on black

section .text
global print_char
global print_string
global print_message
global clear_screen
global set_cursor

; Print a single character
; Input: AL = character, SIL = attribute
print_char:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    
    ; Calculate video memory offset
    mov rcx, [cursor_pos]
    shl rcx, 1
    add rcx, video_memory
    
    ; Write character and attribute
    mov byte [rcx], al      ; Character
    mov byte [rcx + 1], sil ; Attribute
    
    ; Update cursor position
    inc qword [cursor_pos]
    
    ; Check for end of screen
    mov rax, screen_width * screen_height
    cmp qword [cursor_pos], rax
    jl .done
    
    ; Scroll screen if needed
    call scroll_screen
    
.done:
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

; Print a null-terminated string
; Input: RSI = pointer to string
print_string:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    
.loop:
    lodsb
    test al, al
    jz .done
    
    mov sil, [default_attr]
    call print_char
    jmp .loop
    
.done:
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

; Print a message with attribute
; Input: RSI = pointer to string, DIL = attribute
print_message:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    
    mov bl, dil      ; Save attribute
    
.loop:
    lodsb
    test al, al
    jz .done
    
    mov sil, bl
    call print_char
    jmp .loop
    
.done:
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

; Clear the screen
clear_screen:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    
    mov rdi, video_memory
    mov rcx, screen_width * screen_height
    mov al, ' '                  ; Space character
    mov ah, [default_attr]       ; Default attribute
    rep stosw
    
    mov qword [cursor_pos], 0
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

; Set cursor position
; Input: AL = row, SIL = column
set_cursor:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    
    movzx rcx, sil       ; Column
    movzx rax, al        ; Row
    mov rdx, screen_width
    mul rdx
    add rax, rcx
    mov [cursor_pos], rax
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

; Scroll screen up one line
scroll_screen:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    
    ; Move lines up
    mov rsi, video_memory + (screen_width * 2)
    mov rdi, video_memory
    mov rcx, (screen_height - 1) * screen_width
    rep movsw
    
    ; Clear bottom line
    mov rcx, screen_width
    mov al, ' '                  ; Space character
    mov ah, [default_attr]       ; Default attribute
    rep stosw
    
    ; Update cursor position
    sub qword [cursor_pos], screen_width
    
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret 