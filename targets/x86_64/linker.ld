/* NansOS Linker Script
 * Copyright (c) 2025 NansStudios
 */

OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    /* Begin at 1MB - standard for kernel location */
    . = 1M;

    /* Boot section - read-only */
    .boot : {
        /* Ensure multiboot header is at the beginning */
        KEEP(*(.multiboot_header))
        *(.boot)
        *(.boot.*)
    } :boot

    /* Text section - read + execute */
    .text ALIGN(4K) : {
        *(.text._start)
        *(.text.start)
        *(.text)
        *(.text.*)
    } :text

    /* Read-only data */
    .rodata ALIGN(4K) : {
        *(.rodata)
        *(.rodata.*)
    } :rodata

    /* Read-write data (initialized) */
    .data ALIGN(4K) : {
        *(.data)
        *(.data.*)
    } :data

    /* Read-write data (uninitialized) and stack */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
        *(.bss.*)
        /* Reserve 64KB for kernel stack */
        . = ALIGN(16);
        stack_bottom = .;
        . += 64K;
        stack_top = .;
    } :data

    /* Discard unused sections */
    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
    }
}

/* Section permissions */
PHDRS
{
    boot    PT_LOAD FLAGS(5);    /* Read + Execute */
    text    PT_LOAD FLAGS(5);    /* Read + Execute */
    rodata  PT_LOAD FLAGS(4);    /* Read only */
    data    PT_LOAD FLAGS(6);    /* Read + Write */
}
