/**
 * Interrupt Descriptor Table
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* Packed attribute for struct alignment */
#if defined(__GNUC__) || defined(__clang__)
#define PACKED_STRUCT __attribute__((packed))
#else
#define PACKED_STRUCT
#endif

/* Interrupt types */
#define IDT_INTERRUPT_GATE  0x8E
#define IDT_TRAP_GATE      0x8F
#define IDT_TASK_GATE      0x85

/* Number of interrupt vectors */
#define IDT_ENTRIES        256

/* Interrupt vectors */
#define INT_DIVIDE_ERROR   0x00
#define INT_DEBUG          0x01
#define INT_NMI            0x02
#define INT_BREAKPOINT     0x03
#define INT_OVERFLOW       0x04
#define INT_BOUND_RANGE    0x05
#define INT_INVALID_OP     0x06
#define INT_DEVICE_NA      0x07
#define INT_DOUBLE_FAULT   0x08
#define INT_COPROCESSOR    0x09
#define INT_INVALID_TSS    0x0A
#define INT_SEGMENT_NP     0x0B
#define INT_STACK_FAULT    0x0C
#define INT_PROTECTION     0x0D
#define INT_PAGE_FAULT     0x0E
#define INT_FPU_ERROR      0x10
#define INT_ALIGNMENT      0x11
#define INT_MACHINE_CHECK  0x12
#define INT_SIMD_ERROR     0x13
#define INT_VIRT_ERROR     0x14
#define INT_SECURITY       0x1E

/* PIC definitions */
#define PIC1_COMMAND      0x20
#define PIC1_DATA         0x21
#define PIC2_COMMAND      0xA0
#define PIC2_DATA         0xA1

/* PIC initialization command words */
#define ICW1_INIT         0x10
#define ICW1_ICW4         0x01
#define ICW4_8086         0x01

/* Interrupt handler structure */
struct InterruptFrame {
    uint64_t error_code;  /* Error code (if applicable) */
    uint64_t rip;         /* Instruction pointer */
    uint64_t cs;          /* Code segment */
    uint64_t rflags;      /* CPU flags */
    uint64_t rsp;         /* Stack pointer */
    uint64_t ss;          /* Stack segment */
} PACKED_STRUCT;

/* IDT entry structure */
struct IDTEntry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} PACKED_STRUCT;

/* IDT pointer structure */
struct IDTPointer {
    uint16_t limit;
    uint64_t base;
} PACKED_STRUCT;

/* Function declarations */
void idt_init(void);
void idt_set_entry(uint8_t num, uint64_t base, uint8_t flags);
void idt_install(void);
void idt_enable_interrupts(void);
void idt_disable_interrupts(void);

/* Interrupt handler type */
typedef void (*interrupt_handler_t)(struct InterruptFrame* frame);

/* Register interrupt handler */
void idt_register_handler(uint8_t num, interrupt_handler_t handler);

/* PIC functions */
void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);
void pic_send_eoi(uint8_t irq);

/* Default interrupt handlers */
void isr_default_handler(struct InterruptFrame* frame);
void isr_page_fault_handler(struct InterruptFrame* frame);
void isr_keyboard_handler(struct InterruptFrame* frame);
void isr_timer_handler(struct InterruptFrame* frame);
void isr_syscall_handler(struct InterruptFrame* frame);

/* Exception handlers */
void isr_divide_by_zero(struct InterruptFrame* frame);
void isr_debug(struct InterruptFrame* frame);
void isr_nmi(struct InterruptFrame* frame);
void isr_breakpoint(struct InterruptFrame* frame);
void isr_overflow(struct InterruptFrame* frame);
void isr_bound_range(struct InterruptFrame* frame);
void isr_invalid_opcode(struct InterruptFrame* frame);
void isr_device_not_available(struct InterruptFrame* frame);
void isr_double_fault(struct InterruptFrame* frame);
void isr_invalid_tss(struct InterruptFrame* frame);
void isr_segment_not_present(struct InterruptFrame* frame);
void isr_stack_segment(struct InterruptFrame* frame);
void isr_general_protection(struct InterruptFrame* frame);
void isr_fpu_fault(struct InterruptFrame* frame);
void isr_alignment_check(struct InterruptFrame* frame);
void isr_machine_check(struct InterruptFrame* frame);
void isr_simd_exception(struct InterruptFrame* frame);
void isr_virtualization_exception(struct InterruptFrame* frame);

/* IRQ handlers */
void isr_timer(struct InterruptFrame* frame);
void isr_keyboard(struct InterruptFrame* frame);
void isr_cascade(struct InterruptFrame* frame);
void isr_com2(struct InterruptFrame* frame);
void isr_com1(struct InterruptFrame* frame);
void isr_lpt2(struct InterruptFrame* frame);
void isr_floppy(struct InterruptFrame* frame);
void isr_lpt1(struct InterruptFrame* frame);
void isr_rtc(struct InterruptFrame* frame);
void isr_irq9(struct InterruptFrame* frame);
void isr_irq10(struct InterruptFrame* frame);
void isr_irq11(struct InterruptFrame* frame);
void isr_mouse(struct InterruptFrame* frame);
void isr_coproc(struct InterruptFrame* frame);
void isr_primary_ata(struct InterruptFrame* frame);
void isr_secondary_ata(struct InterruptFrame* frame); 