[BITS 16]
; NansOS Installation Kernel

section .text
global _start

_start:
    ; Set up segments
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000

    ; Save drive info
    mov [bootDrive], dl    ; Source drive
    mov [driveCount], dh   ; Number of drives

    ; Clear screen
    mov ax, 0x0003
    int 0x10

    ; Print welcome message
    mov si, msg_welcome
    call print_string

    ; Show installation menu
    call show_menu

    ; Wait for drive selection
    call get_key
    sub al, '0'
    cmp al, [driveCount]
    jae .invalid_drive

    ; Save target drive
    add al, 0x80        ; Convert to drive number
    mov [targetDrive], al

    ; Confirm installation
    mov si, msg_confirm
    call print_string
    call get_key
    cmp al, 'Y'
    je .start_install
    cmp al, 'y'
    je .start_install
    jmp .cancel_install

.start_install:
    ; Show progress
    mov si, msg_installing
    call print_string

    ; Format target drive
    mov dl, [targetDrive]
    call format_drive
    jc .install_error

    ; Copy bootloader
    call copy_bootloader
    jc .install_error

    ; Copy kernel
    call copy_kernel
    jc .install_error

    ; Installation complete
    mov si, msg_success
    call print_string
    jmp .reboot

.invalid_drive:
    mov si, msg_invalid
    call print_string
    jmp .cancel_install

.install_error:
    mov si, msg_error
    call print_string

.cancel_install:
    mov si, msg_cancelled
    call print_string

.reboot:
    mov si, msg_reboot
    call print_string
    call get_key
    int 0x19        ; Reboot

; Show installation menu
show_menu:
    mov si, msg_menu
    call print_string
    ret

; Get keyboard input
get_key:
    mov ah, 0
    int 0x16
    ret

; Format target drive
format_drive:
    ; TODO: Implement drive formatting
    clc             ; Clear carry (success)
    ret

; Copy bootloader to target drive
copy_bootloader:
    ; TODO: Implement bootloader copy
    clc
    ret

; Copy kernel to target drive
copy_kernel:
    ; TODO: Implement kernel copy
    clc
    ret

; Print string (SI = string pointer)
print_string:
    pusha
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x0A    ; Light green
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    popa
    ret

section .data
bootDrive:      db 0    ; Source (USB) drive
targetDrive:    db 0    ; Target (HDD) drive
driveCount:     db 0    ; Number of available drives

; Messages
msg_welcome:    db 'NansOS Installation Program', 13, 10
                db '=========================', 0
msg_menu:       db 'Select target drive:', 13, 10
                db '0) First Hard Drive', 13, 10
                db '1) Second Hard Drive', 13, 10
                db '2) Third Hard Drive', 0
msg_confirm:    db 'WARNING: This will erase ALL data on the selected drive!', 13, 10
                db 'Continue? (Y/N): ', 0
msg_installing: db 'Installing NansOS...', 0
msg_success:    db 'Installation complete!', 0
msg_error:      db 'Installation failed!', 0
msg_invalid:    db 'Invalid drive selection!', 0
msg_cancelled:  db 'Installation cancelled.', 0
msg_reboot:     db 'Press any key to reboot...', 0 