; GUI Constants and Structures for NansOS

; Window styles
%define WINDOW_STYLE_TERMINAL   0x01
%define WINDOW_STYLE_DIALOG     0x02
%define WINDOW_STYLE_MENU       0x04
%define WINDOW_STYLE_TASKBAR    0x08

; Colors
%define COLOR_BLACK             0x00
%define COLOR_GREEN             0x0A
%define COLOR_BRIGHT_GREEN      0x0B
%define COLOR_GRAY              0x07

; Window structure (64-bit aligned)
struc Window
    .id:        resq 1    ; Window ID
    .style:     resq 1    ; Window style
    .x:         resq 1    ; X position
    .y:         resq 1    ; Y position
    .width:     resq 1    ; Width
    .height:    resq 1    ; Height
    .title:     resb 32   ; Window title
    align 8
    .content:   resq 1    ; Pointer to content buffer
    .next:      resq 1    ; Next window in list
endstruc

; Shared variables
extern window_list
extern active_window
extern current_x
extern current_y 