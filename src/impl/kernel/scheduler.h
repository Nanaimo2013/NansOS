/**
 * Task Scheduler
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* Task states */
#define TASK_READY      0
#define TASK_RUNNING    1
#define TASK_BLOCKED    2
#define TASK_SLEEPING   3
#define TASK_ZOMBIE     4

/* Task priorities */
#define PRIORITY_LOW    0
#define PRIORITY_NORMAL 1
#define PRIORITY_HIGH   2
#define PRIORITY_KERNEL 3

/* Maximum number of tasks */
#define MAX_TASKS       256

/* Task flags */
#define TASK_KERNEL     (1 << 0)
#define TASK_USER       (1 << 1)
#define TASK_INTERRUPT  (1 << 2)

/* CPU context structure */
struct CPUContext {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip, cs;
    uint64_t rflags;
    uint64_t rsp, ss;
};

/* Task structure */
struct Task {
    uint32_t id;                    /* Task ID */
    char name[32];                  /* Task name */
    uint32_t state;                 /* Task state */
    uint32_t priority;              /* Task priority */
    uint32_t flags;                 /* Task flags */
    uint64_t stack;                 /* Stack pointer */
    uint64_t stack_size;            /* Stack size */
    struct CPUContext context;      /* CPU context */
    uint64_t page_directory;        /* Page directory */
    uint64_t sleep_until;          /* Wake up time for sleeping tasks */
    struct Task* next;             /* Next task in queue */
};

/* Scheduler functions */
void scheduler_init(void);
void scheduler_start(void);
void scheduler_stop(void);
void scheduler_yield(void);
void scheduler_tick(void);

/* Task management */
struct Task* task_create(const char* name, void (*entry)(void), uint32_t priority, uint32_t flags);
void task_destroy(struct Task* task);
void task_sleep(uint32_t milliseconds);
void task_wake(struct Task* task);
void task_block(struct Task* task);
void task_unblock(struct Task* task);
void task_exit(int status);

/* Task switching */
void task_switch(struct Task* next);
struct Task* task_current(void);
uint32_t task_count(void);

/* Task information */
void task_list(void);
struct Task* task_find(uint32_t id);
struct Task* task_find_by_name(const char* name);

/* Priority management */
void task_set_priority(struct Task* task, uint32_t priority);
uint32_t task_get_priority(struct Task* task); 