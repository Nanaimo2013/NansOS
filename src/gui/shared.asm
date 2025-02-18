[BITS 64]
; Shared GUI variables for NansOS

section .data
global window_list
global active_window
global current_x
global current_y

window_list:     dq 0    ; Linked list of windows
active_window:   dq 0    ; Currently active window
current_x:       dq 0    ; Current cursor X position
current_y:       dq 0    ; Current cursor Y position 