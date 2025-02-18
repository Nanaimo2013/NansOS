[BITS 32]
; Desktop Manager for NansOS
; Implements Fallout-style desktop interface

%include "src/gui/gui.inc"

extern window_init
extern window_create
extern window_destroy
extern window_draw
extern window_handle_input
extern taskbar_update

; Desktop states
DESKTOP_STATE_BOOT      equ 0
DESKTOP_STATE_LOGIN     equ 1
DESKTOP_STATE_DESKTOP   equ 2
DESKTOP_STATE_MENU      equ 3

section .data
    desktop_state:      dd DESKTOP_STATE_BOOT
    boot_progress:      dd 0
    login_attempts:     dd 3
    current_user:       times 32 db 0
    
    ; Boot messages
    boot_msg:       db 'ROBCO INDUSTRIES UNIFIED OPERATING SYSTEM', 13, 10
                    db 'COPYRIGHT 2024 ROBCO INDUSTRIES', 13, 10
                    db 'LOADER V1.1', 13, 10, 0
    
    init_msg:       db 'INITIALIZING SYSTEM...', 0
    mem_msg:        db 'DETECTING MEMORY...', 0
    cpu_msg:        db 'DETECTING CPU...', 0
    disk_msg:       db 'DETECTING STORAGE DEVICES...', 0
    done_msg:       db 'SYSTEM INITIALIZATION COMPLETE', 0
    
    ; Login messages
    login_msg:      db 'ENTER PASSWORD:', 0
    login_error:    db 'INCORRECT PASSWORD - ', 0
    login_locked:   db 'TERMINAL LOCKED - CONTACT ADMINISTRATOR', 0
    
    ; Desktop elements
    menu_items:     db 'FILE MANAGER', 0
                    db 'TERMINAL', 0
                    db 'SYSTEM INFO', 0
                    db 'SETTINGS', 0
                    db 'SHUTDOWN', 0
    
    taskbar_items:  db 'START', 0
                    db 'CPU: ', 0
                    db 'MEM: ', 0
                    db 'DISK: ', 0
                    db 'TIME: ', 0

section .text
global desktop_init
global desktop_update
global desktop_handle_input

; Initialize desktop
desktop_init:
    push ebp
    mov ebp, esp
    
    ; Initialize window system
    call window_init
    
    ; Start boot sequence
    mov dword [desktop_state], DESKTOP_STATE_BOOT
    mov dword [boot_progress], 0
    
    mov esp, ebp
    pop ebp
    ret

; Update desktop
desktop_update:
    push ebp
    mov ebp, esp
    
    ; Handle current state
    mov eax, [desktop_state]
    cmp eax, DESKTOP_STATE_BOOT
    je .update_boot
    cmp eax, DESKTOP_STATE_LOGIN
    je .update_login
    cmp eax, DESKTOP_STATE_DESKTOP
    je .update_desktop
    cmp eax, DESKTOP_STATE_MENU
    je .update_menu
    
    jmp .done
    
.update_boot:
    call update_boot_sequence
    jmp .done
    
.update_login:
    call update_login_screen
    jmp .done
    
.update_desktop:
    call update_desktop_screen
    jmp .done
    
.update_menu:
    call update_menu_screen
    
.done:
    ; Update taskbar
    call taskbar_update
    
    mov esp, ebp
    pop ebp
    ret

; Update boot sequence
update_boot_sequence:
    push ebp
    mov ebp, esp
    
    ; Show boot messages
    mov eax, [boot_progress]
    cmp eax, 0
    je .show_boot_msg
    cmp eax, 1
    je .show_init_msg
    cmp eax, 2
    je .show_mem_msg
    cmp eax, 3
    je .show_cpu_msg
    cmp eax, 4
    je .show_disk_msg
    cmp eax, 5
    je .show_done_msg
    
    ; Boot sequence complete
    mov dword [desktop_state], DESKTOP_STATE_LOGIN
    jmp .done
    
.show_boot_msg:
    push boot_msg
    call print_message
    add esp, 4
    inc dword [boot_progress]
    jmp .done
    
.show_init_msg:
    push init_msg
    call print_message
    add esp, 4
    inc dword [boot_progress]
    jmp .done
    
.show_mem_msg:
    push mem_msg
    call print_message
    add esp, 4
    inc dword [boot_progress]
    jmp .done
    
.show_cpu_msg:
    push cpu_msg
    call print_message
    add esp, 4
    inc dword [boot_progress]
    jmp .done
    
.show_disk_msg:
    push disk_msg
    call print_message
    add esp, 4
    inc dword [boot_progress]
    jmp .done
    
.show_done_msg:
    push done_msg
    call print_message
    add esp, 4
    inc dword [boot_progress]
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Update login screen
update_login_screen:
    push ebp
    mov ebp, esp
    
    ; Check login attempts
    mov eax, [login_attempts]
    test eax, eax
    jz .show_locked
    
    ; Show login prompt
    push login_msg
    call print_message
    add esp, 4
    jmp .done
    
.show_locked:
    push login_locked
    call print_message
    add esp, 4
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Update desktop screen
update_desktop_screen:
    push ebp
    mov ebp, esp
    
    ; Draw desktop windows
    mov esi, [window_list]
.draw_windows:
    test esi, esi
    jz .done
    
    push esi
    call window_draw
    add esp, 4
    
    mov esi, [esi + Window.next]
    jmp .draw_windows
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Update menu screen
update_menu_screen:
    push ebp
    mov ebp, esp
    
    ; Draw menu items
    mov esi, menu_items
    xor ecx, ecx
.draw_items:
    cmp byte [esi], 0
    je .done
    
    push ecx
    push esi
    call draw_menu_item
    add esp, 8
    
    ; Find next item
.find_next:
    inc esi
    cmp byte [esi], 0
    jne .find_next
    inc esi
    inc ecx
    jmp .draw_items
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Draw menu item
draw_menu_item:
    push ebp
    mov ebp, esp
    
    ; Parameters:
    ; ebp + 8  = item text
    ; ebp + 12 = item index
    
    ; Calculate position
    mov eax, [ebp + 12]
    mov edx, 80                     ; Screen width
    mul edx
    add eax, 2                      ; Indent
    
    ; Set cursor position
    push eax
    call set_cursor
    add esp, 4
    
    ; Print item
    push dword [ebp + 8]
    call print_message
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Print message
print_message:
    push ebp
    mov ebp, esp
    
    mov esi, [ebp + 8]             ; Message pointer
.print_loop:
    mov al, [esi]
    test al, al
    jz .done
    
    push eax
    call print_char
    add esp, 4
    
    inc esi
    jmp .print_loop
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Print character
print_char:
    push ebp
    mov ebp, esp
    
    ; Calculate screen position
    mov eax, [cursor_y]
    mov edx, 80
    mul edx
    add eax, [cursor_x]
    shl eax, 1
    add eax, 0xB8000
    
    ; Write character
    mov dl, [ebp + 8]
    mov dh, 0x0A                    ; Light green on black
    mov [eax], dx
    
    ; Update cursor
    inc dword [cursor_x]
    cmp dword [cursor_x], 80
    jl .done
    mov dword [cursor_x], 0
    inc dword [cursor_y]
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Set cursor position
set_cursor:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp + 8]
    mov edx, 80
    div edx
    mov [cursor_y], eax
    mov [cursor_x], edx
    
    mov esp, ebp
    pop ebp
    ret

section .data
    cursor_x:    dd 0
    cursor_y:    dd 0 