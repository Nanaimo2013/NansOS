/* Linker script for NansOS 32-bit */
OUTPUT_FORMAT(pei-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text : {
        *(.text)
        *(.text.*)
        . = ALIGN(4096);
    }

    .data : {
        *(.data)
        *(.data.*)
        . = ALIGN(4096);
    }

    .rdata : {
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4096);
    }

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4096);
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
} 