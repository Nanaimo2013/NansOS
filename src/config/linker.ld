/* Linker script for NansOS kernel */
OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text : {
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(16) : {
        *(.rodata)
        *(.rodata.*)
    }

    .data ALIGN(16) : {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(16) : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
    }
} 