[BITS 32]
; File Manager for NansOS
; Implements Fallout-style file browser

%include "src/gui/gui.inc"

extern window_create
extern window_destroy
extern window_draw
extern fs_list_directory
extern fs_change_directory
extern fs_create_file
extern fs_delete_file
extern print_char
extern print_message

; File manager states
FM_STATE_BROWSE    equ 0
FM_STATE_COPY      equ 1
FM_STATE_MOVE      equ 2
FM_STATE_DELETE    equ 3
FM_STATE_CREATE    equ 4

section .data
    fm_window:          dd 0        ; File manager window
    fm_state:           dd 0        ; Current state
    current_path:       times 256 db 0
    selected_file:      times 13 db 0
    clipboard_path:     times 256 db 0
    
    ; Window titles
    fm_title:           db 'FILE MANAGER - ROBCO TERMINAL', 0
    browse_title:       db 'BROWSE FILES', 0
    copy_title:         db 'COPY FILE', 0
    move_title:         db 'MOVE FILE', 0
    delete_title:       db 'DELETE FILE', 0
    create_title:       db 'CREATE FILE', 0
    
    ; Messages
    path_prompt:        db 'CURRENT PATH: ', 0
    select_prompt:      db 'SELECT FILE: ', 0
    confirm_msg:        db 'CONFIRM? (Y/N)', 0
    name_prompt:        db 'ENTER NAME: ', 0
    error_msg:          db 'ERROR - OPERATION FAILED', 0
    
    ; Commands
    cmd_copy:           db 'COPY', 0
    cmd_move:           db 'MOVE', 0
    cmd_delete:         db 'DELETE', 0
    cmd_create:         db 'CREATE', 0
    cmd_exit:           db 'EXIT', 0

section .text
global fm_init
global fm_update
global fm_handle_input
global fm_cleanup

; Initialize file manager
fm_init:
    push ebp
    mov ebp, esp
    
    ; Create main window
    push dword WINDOW_STYLE_TERMINAL
    push dword 20                    ; Height
    push dword 60                    ; Width
    push dword 2                     ; Y
    push dword 10                    ; X
    push fm_title
    call window_create
    add esp, 24
    mov [fm_window], eax
    
    ; Initialize state
    mov dword [fm_state], FM_STATE_BROWSE
    
    ; Set initial path to root
    mov byte [current_path], '/'
    mov byte [current_path + 1], 0
    
    mov esp, ebp
    pop ebp
    ret

; Update file manager
fm_update:
    push ebp
    mov ebp, esp
    
    ; Handle current state
    mov eax, [fm_state]
    cmp eax, FM_STATE_BROWSE
    je .update_browse
    cmp eax, FM_STATE_COPY
    je .update_copy
    cmp eax, FM_STATE_MOVE
    je .update_move
    cmp eax, FM_STATE_DELETE
    je .update_delete
    cmp eax, FM_STATE_CREATE
    je .update_create
    
    jmp .done
    
.update_browse:
    call update_browse_view
    jmp .done
    
.update_copy:
    call update_copy_view
    jmp .done
    
.update_move:
    call update_move_view
    jmp .done
    
.update_delete:
    call update_delete_view
    jmp .done
    
.update_create:
    call update_create_view
    
.done:
    ; Draw window
    push dword [fm_window]
    call window_draw
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Update browse view
update_browse_view:
    push ebp
    mov ebp, esp
    
    ; Show current path
    push path_prompt
    call print_message
    add esp, 4
    
    push current_path
    call print_message
    add esp, 4
    
    ; List directory contents
    push current_path
    call fs_list_directory
    add esp, 4
    
    ; Show commands
    mov esi, cmd_copy
    call print_command
    mov esi, cmd_move
    call print_command
    mov esi, cmd_delete
    call print_command
    mov esi, cmd_create
    call print_command
    mov esi, cmd_exit
    call print_command
    
    mov esp, ebp
    pop ebp
    ret

; Update copy view
update_copy_view:
    push ebp
    mov ebp, esp
    
    ; Show selected file
    push select_prompt
    call print_message
    add esp, 4
    
    push selected_file
    call print_message
    add esp, 4
    
    ; Show destination prompt
    push path_prompt
    call print_message
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Update move view
update_move_view:
    push ebp
    mov ebp, esp
    
    ; Show selected file
    push select_prompt
    call print_message
    add esp, 4
    
    push selected_file
    call print_message
    add esp, 4
    
    ; Show destination prompt
    push path_prompt
    call print_message
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Update delete view
update_delete_view:
    push ebp
    mov ebp, esp
    
    ; Show selected file
    push select_prompt
    call print_message
    add esp, 4
    
    push selected_file
    call print_message
    add esp, 4
    
    ; Show confirmation
    push confirm_msg
    call print_message
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Update create view
update_create_view:
    push ebp
    mov ebp, esp
    
    ; Show name prompt
    push name_prompt
    call print_message
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Print command
print_command:
    push ebp
    mov ebp, esp
    
    ; Print bracket
    mov al, '['
    push eax
    call print_char
    add esp, 4
    
    ; Print command text
    push esi
    call print_message
    add esp, 4
    
    ; Print closing bracket
    mov al, ']'
    push eax
    call print_char
    add esp, 4
    
    ; Print space
    mov al, ' '
    push eax
    call print_char
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret

; Handle file manager input
fm_handle_input:
    push ebp
    mov ebp, esp
    
    ; Get input character
    mov al, [ebp + 8]
    
    ; Handle based on current state
    mov edx, [fm_state]
    cmp edx, FM_STATE_BROWSE
    je .handle_browse
    cmp edx, FM_STATE_COPY
    je .handle_copy
    cmp edx, FM_STATE_MOVE
    je .handle_move
    cmp edx, FM_STATE_DELETE
    je .handle_delete
    cmp edx, FM_STATE_CREATE
    je .handle_create
    
    jmp .done
    
.handle_browse:
    ; Handle browse mode commands
    cmp al, 'c'
    je .start_copy
    cmp al, 'm'
    je .start_move
    cmp al, 'd'
    je .start_delete
    cmp al, 'n'
    je .start_create
    cmp al, 'x'
    je .exit
    jmp .done
    
.handle_copy:
    ; Handle copy mode input
    ; TODO: Implement copy handling
    jmp .done
    
.handle_move:
    ; Handle move mode input
    ; TODO: Implement move handling
    jmp .done
    
.handle_delete:
    ; Handle delete confirmation
    cmp al, 'y'
    je .confirm_delete
    cmp al, 'n'
    je .cancel_delete
    jmp .done
    
.handle_create:
    ; Handle create mode input
    ; TODO: Implement create handling
    jmp .done
    
.start_copy:
    mov dword [fm_state], FM_STATE_COPY
    jmp .done
    
.start_move:
    mov dword [fm_state], FM_STATE_MOVE
    jmp .done
    
.start_delete:
    mov dword [fm_state], FM_STATE_DELETE
    jmp .done
    
.start_create:
    mov dword [fm_state], FM_STATE_CREATE
    jmp .done
    
.confirm_delete:
    push selected_file
    call fs_delete_file
    add esp, 4
    mov dword [fm_state], FM_STATE_BROWSE
    jmp .done
    
.cancel_delete:
    mov dword [fm_state], FM_STATE_BROWSE
    jmp .done
    
.exit:
    push dword [fm_window]
    call window_destroy
    add esp, 4
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Clean up file manager
fm_cleanup:
    push ebp
    mov ebp, esp
    
    ; Destroy window if it exists
    mov eax, [fm_window]
    test eax, eax
    jz .done
    
    push eax
    call window_destroy
    add esp, 4
    
.done:
    mov esp, ebp
    pop ebp
    ret 