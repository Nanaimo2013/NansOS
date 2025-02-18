[BITS 64]
; Display Driver for NansOS
; Supports VGA text mode and graphics mode

; VGA registers
VGA_AC_INDEX      equ 0x3C0
VGA_AC_WRITE      equ 0x3C0
VGA_AC_READ       equ 0x3C1
VGA_MISC_WRITE    equ 0x3C2
VGA_SEQ_INDEX     equ 0x3C4
VGA_SEQ_DATA      equ 0x3C5
VGA_DAC_READ_INDEX  equ 0x3C7
VGA_DAC_WRITE_INDEX equ 0x3C8
VGA_DAC_DATA      equ 0x3C9
VGA_MISC_READ     equ 0x3CC
VGA_GC_INDEX      equ 0x3CE
VGA_GC_DATA       equ 0x3CF
VGA_CRTC_INDEX    equ 0x3D4
VGA_CRTC_DATA     equ 0x3D5
VGA_INSTAT_READ   equ 0x3DA

; VGA colors
VGA_BLACK         equ 0x0
VGA_BLUE          equ 0x1
VGA_GREEN         equ 0x2
VGA_CYAN          equ 0x3
VGA_RED           equ 0x4
VGA_MAGENTA       equ 0x5
VGA_BROWN         equ 0x6
VGA_LIGHT_GREY    equ 0x7
VGA_DARK_GREY     equ 0x8
VGA_LIGHT_BLUE    equ 0x9
VGA_LIGHT_GREEN   equ 0xA
VGA_LIGHT_CYAN    equ 0xB
VGA_LIGHT_RED     equ 0xC
VGA_LIGHT_MAGENTA equ 0xD
VGA_YELLOW        equ 0xE
VGA_WHITE         equ 0xF

section .data
    vga_width:      dq 80          ; Default text mode width
    vga_height:     dq 25          ; Default text mode height
    vga_mode:       db 0           ; 0 = text mode, 1 = graphics mode
    cursor_x:       dq 0           ; Cursor X position
    cursor_y:       dq 0           ; Cursor Y position
    vga_buffer:     dq 0xB8000     ; Default video memory address

section .text
global display_init
global display_clear
global display_set_pixel
global display_draw_line
global display_print_char
global display_print_string
global display_set_cursor
global display_scroll

display_init:
    push rbp
    mov rbp, rsp
    
    ; Set up text mode 80x25
    mov rax, 0x0003     ; Text mode 3
    int 0x10
    
    ; Set cursor shape
    mov dx, VGA_CRTC_INDEX
    mov al, 0x0A
    out dx, al
    inc dx
    mov al, 0x0D
    out dx, al
    
    dec dx
    mov al, 0x0B
    out dx, al
    inc dx
    mov al, 0x0E
    out dx, al
    
    ; Clear screen
    call display_clear
    
    mov rsp, rbp
    pop rbp
    ret

display_clear:
    push rbp
    mov rbp, rsp
    
    mov rdi, [vga_buffer]
    mov rcx, 2000      ; 80 * 25
    mov ax, 0x0720     ; White on black space
    rep stosw
    
    mov qword [cursor_x], 0
    mov qword [cursor_y], 0
    call display_set_cursor
    
    mov rsp, rbp
    pop rbp
    ret

display_set_pixel:
    push rbp
    mov rbp, rsp
    
    ; Check if in graphics mode
    cmp byte [vga_mode], 1
    jne .done
    
    ; Calculate pixel offset
    mov rax, [rbp + 16]    ; Y coordinate
    mov rdx, 320           ; Assume 320x200 mode
    mul rdx
    add rax, [rbp + 12]     ; X coordinate
    mov rdi, 0xA0000       ; VGA memory
    add rdi, rax
    
    ; Set pixel color
    mov al, [rbp + 20]     ; Color
    mov [rdi], al
    
.done:
    mov rsp, rbp
    pop rbp
    ret

display_draw_line:
    push rbp
    mov rbp, rsp
    push rsi
    push rdi
    push rax
    push rbx
    push rcx
    push rdx
    
    ; Bresenham's line algorithm
    mov rsi, [rbp + 16]     ; x1
    mov rdi, [rbp + 20]    ; y1
    mov rax, [rbp + 24]    ; x2
    mov rcx, [rbp + 28]    ; y2
    
    ; Calculate deltas
    mov rbx, rax
    sub rbx, rsi           ; dx
    mov rdx, rcx
    sub rdx, rdi           ; dy
    
    ; TODO: Implement line drawing algorithm
    
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rdi
    pop rsi
    mov rsp, rbp
    pop rbp
    ret

display_print_char:
    push rbp
    mov rbp, rsp
    
    ; Get character and attributes
    mov al, [rbp + 8]      ; Character
    mov sil, [rbp + 16]     ; Attributes
    
    ; Calculate screen offset
    mov rax, [cursor_y]
    mov rdx, [vga_width]
    mul rdx
    add rax, [cursor_x]
    shl rax, 1
    add rax, [vga_buffer]
    
    ; Write character and attributes
    mov [rax], ax
    
    ; Update cursor
    inc qword [cursor_x]
    mov rax, [vga_width]
    cmp qword [cursor_x], rax
    jl .done
    
    mov qword [cursor_x], 0
    inc qword [cursor_y]
    
    mov rax, [vga_height]
    cmp qword [cursor_y], rax
    jl .done
    
    call display_scroll
    dec qword [cursor_y]
    
.done:
    call display_set_cursor
    mov rsp, rbp
    pop rbp
    ret

display_print_string:
    push rbp
    mov rbp, rsp
    push rsi
    
    mov rsi, [rbp + 8]     ; String pointer
    mov sil, [rbp + 16]     ; Attributes
    
.loop:
    lodsb
    test al, al
    jz .done
    
    mov al, sil
    call display_print_char
    jmp .loop
    
.done:
    pop rsi
    mov rsp, rbp
    pop rbp
    ret

display_set_cursor:
    push rbp
    mov rbp, rsp
    
    ; Calculate cursor position
    movzx rax, al
    mov rdx, [vga_width]
    mul rdx
    movzx rcx, sil
    add rax, rcx
    
    ; Set low cursor byte
    mov dx, VGA_CRTC_INDEX
    mov al, 0x0F
    out dx, al
    inc dx
    mov al, bl
    out dx, al
    
    ; Set high cursor byte
    dec dx
    mov al, 0x0E
    out dx, al
    inc dx
    mov al, bh
    out dx, al
    
    mov rsp, rbp
    pop rbp
    ret

display_scroll:
    push rbp
    mov rbp, rsp
    
    ; Move lines up
    mov rsi, [vga_buffer]
    add rsi, [vga_width]
    add rsi, [vga_width]  ; Skip first line
    mov rdi, [vga_buffer]
    mov rcx, [vga_height]
    dec rcx               ; One less than total height
    imul rcx, [vga_width]
    rep movsw
    
    ; Clear bottom line
    mov rcx, [vga_width]
    mov ax, 0x0720       ; White on black space
    rep stosw
    
    mov rsp, rbp
    pop rbp
    ret 