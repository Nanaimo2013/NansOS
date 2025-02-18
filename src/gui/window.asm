[BITS 64]
; Window Manager for NansOS
; Implements Fallout-style terminal interface

%include "src/gui/gui.inc"

; External functions
extern malloc
extern free
extern get_cpu_info
extern get_memory_info
extern get_disk_info
extern get_time_info
extern print_char
extern print_string

; External variables
extern window_list
extern active_window
extern current_x
extern current_y

section .data
    next_window_id:  dq 1
    taskbar:         dq 0
    desktop_title:   db 'ROBCO INDUSTRIES UNIFIED OPERATING SYSTEM', 0
    taskbar_title:   db 'SYSTEM STATUS', 0
    menu_title:      db 'MAIN MENU', 0
    cpu_info:        db 'CPU: ', 0
    memory_info:     db 'MEM: ', 0
    disk_info:       db 'DISK: ', 0
    time_info:       db 'TIME: ', 0

section .text
global window_init
global window_create
global window_destroy
global window_draw
global window_handle_input
global window_show_desktop
global taskbar_update

; Initialize window system
window_init:
    push rbp
    mov rbp, rsp
    
    ; Initialize window list
    mov qword [window_list], 0
    mov qword [active_window], 0
    mov qword [current_x], 0
    mov qword [current_y], 0
    
    ; Create taskbar
    push dword WINDOW_STYLE_TASKBAR
    push dword 24                    ; Height
    push dword 80                    ; Width
    push dword 0                     ; Y
    push dword 0                     ; X
    push taskbar_title
    call window_create
    add esp, 24
    mov [taskbar], rax
    
    ; Create desktop
    push dword WINDOW_STYLE_TERMINAL
    push dword 23                    ; Height (full screen - taskbar)
    push dword 80                    ; Width
    push dword 1                     ; Y (below taskbar)
    push dword 0                     ; X
    push desktop_title
    call window_create
    add esp, 24
    
    mov rsp, rbp
    pop rbp
    ret

; Create new window
; Input: RDI = title, RSI = x, RDX = y, RCX = width, R8 = height, R9 = style
window_create:
    push rbp
    mov rbp, rsp
    
    ; Allocate window structure
    push rcx
    mov rcx, Window_size
    call malloc
    pop rcx
    test rax, rax
    jz .fail
    
    ; Initialize window
    mov r10, rax                ; Save window pointer
    
    ; Set window ID
    mov rax, [next_window_id]
    mov [r10 + Window.id], rax
    inc qword [next_window_id]
    
    ; Copy title (max 31 chars + null)
    mov rax, r10
    add rax, Window.title
    mov r11, rdi                ; Source (title)
    mov rcx, 31
.copy_title:
    mov dl, [r11]
    test dl, dl
    jz .title_done
    mov [rax], dl
    inc rax
    inc r11
    loop .copy_title
.title_done:
    mov byte [rax], 0           ; Null terminate
    
    ; Set window properties
    mov [r10 + Window.x], rsi
    mov [r10 + Window.y], rdx
    mov [r10 + Window.width], rcx
    mov [r10 + Window.height], r8
    mov [r10 + Window.style], r9
    
    ; Allocate content buffer
    mov rax, rcx                ; Width
    mul r8                      ; Height
    shl rax, 1                  ; 2 bytes per character
    push rcx
    mov rcx, rax
    call malloc
    pop rcx
    test rax, rax
    jz .fail_content
    
    mov [r10 + Window.content], rax
    
    ; Add to window list
    mov rax, [window_list]
    mov [r10 + Window.next], rax
    mov [window_list], r10
    
    mov rax, r10                ; Return window pointer
    jmp .done
    
.fail_content:
    push r10
    call free
    pop r10
    xor rax, rax
    jmp .done
    
.fail:
    xor rax, rax
    
.done:
    mov rsp, rbp
    pop rbp
    ret

; Destroy window
; Input: RDI = window pointer
window_destroy:
    push rbp
    mov rbp, rsp
    
    ; Save window pointer
    mov r10, rdi
    
    ; Remove from window list
    mov rax, [window_list]
    cmp rax, r10
    je .is_first
    
    ; Find window in list
.find_loop:
    test rax, rax
    jz .done
    cmp [rax + Window.next], r10
    je .found
    mov rax, [rax + Window.next]
    jmp .find_loop
    
.found:
    mov rcx, [r10 + Window.next]
    mov [rax + Window.next], rcx
    jmp .free
    
.is_first:
    mov rax, [r10 + Window.next]
    mov [window_list], rax
    
.free:
    ; Free content buffer
    mov rdi, [r10 + Window.content]
    call free
    
    ; Free window structure
    mov rdi, r10
    call free
    
.done:
    mov rsp, rbp
    pop rbp
    ret

; Draw window
; Input: RDI = window pointer
window_draw:
    push rbp
    mov rbp, rsp
    
    ; Save window pointer
    mov r10, rdi
    
    ; Draw border
    mov rdi, r10
    call draw_border
    
    ; Draw content
    mov rdi, r10
    call draw_content
    
    mov rsp, rbp
    pop rbp
    ret

; Draw window border
; Input: RDI = window pointer
draw_border:
    push rbp
    mov rbp, rsp
    
    ; Calculate screen position
    mov rax, [rdi + Window.y]
    mov rdx, 80
    mul rdx
    add rax, [rdi + Window.x]
    shl rax, 1
    add rax, 0xB8000
    
    ; Draw top border
    mov rcx, [rdi + Window.width]
    mov rdx, rax
    mov al, 0xC9                ; Top-left corner
    mov ah, 0x07                ; Light gray on black
    mov [rdx], ax
    add rdx, 2
    dec rcx
    mov al, 0xCD                ; Horizontal line
.top_loop:
    mov [rdx], ax
    add rdx, 2
    loop .top_loop
    mov al, 0xBB                ; Top-right corner
    mov [rdx], ax
    
    ; Draw sides
    mov rcx, [rdi + Window.height]
    dec rcx
.side_loop:
    add rax, 160                ; Next line
    mov rdx, rax
    mov al, 0xBA                ; Vertical line
    mov [rdx], ax
    add rdx, [rdi + Window.width]
    add rdx, [rdi + Window.width]
    sub rdx, 2
    mov [rdx], ax
    loop .side_loop
    
    ; Draw bottom border
    add rax, 160
    mov rdx, rax
    mov al, 0xC8                ; Bottom-left corner
    mov [rdx], ax
    add rdx, 2
    mov rcx, [rdi + Window.width]
    dec rcx
    mov al, 0xCD                ; Horizontal line
.bottom_loop:
    mov [rdx], ax
    add rdx, 2
    loop .bottom_loop
    mov al, 0xBC                ; Bottom-right corner
    mov [rdx], ax
    
    mov rsp, rbp
    pop rbp
    ret

; Draw window content
; Input: RDI = window pointer
draw_content:
    push rbp
    mov rbp, rsp
    
    ; Calculate content area start
    mov rax, [rdi + Window.y]
    inc rax
    mov rdx, 80
    mul rdx
    add rax, [rdi + Window.x]
    inc rax
    shl rax, 1
    add rax, 0xB8000
    
    ; Copy content
    mov rsi, [rdi + Window.content]
    mov rcx, [rdi + Window.height]
    dec rcx
    dec rcx
.line_loop:
    push rcx
    mov rcx, [rdi + Window.width]
    dec rcx
    dec rcx
    push rax
.char_loop:
    mov dx, [rsi]
    mov [rax], dx
    add rsi, 2
    add rax, 2
    loop .char_loop
    pop rax
    add rax, 160
    pop rcx
    loop .line_loop
    
    mov rsp, rbp
    pop rbp
    ret

; Handle window input
; Input: RDI = window pointer, RSI = input character
window_handle_input:
    push rbp
    mov rbp, rsp
    
    ; TODO: Implement input handling
    
    mov rsp, rbp
    pop rbp
    ret

; Show desktop
window_show_desktop:
    push rbp
    mov rbp, rsp
    
    ; Clear screen
    mov rdi, 0xB8000
    mov rcx, 2000
    mov ax, 0x0720
    rep stosw
    
    ; Draw all windows
    mov rdi, [window_list]
.draw_loop:
    test rdi, rdi
    jz .done
    call window_draw
    mov rdi, [rdi + Window.next]
    jmp .draw_loop
    
.done:
    mov rsp, rbp
    pop rbp
    ret

; Update taskbar
taskbar_update:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    
    mov rsi, [taskbar]
    test rsi, rsi
    jz .done
    
    ; Update CPU info
    call get_cpu_info
    
    ; Update memory info
    call get_memory_info
    
    ; Update disk info
    call get_disk_info
    
    ; Update time
    call get_time_info
    
    ; Redraw taskbar
    mov rdi, rsi
    call window_draw
    
.done:
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret 