/**
 * System Information Gathering
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* CPU Feature flags */
#define CPU_FEATURE_FPU    (1 << 0)
#define CPU_FEATURE_MMX    (1 << 1)
#define CPU_FEATURE_SSE    (1 << 2)
#define CPU_FEATURE_SSE2   (1 << 3)
#define CPU_FEATURE_SSE3   (1 << 4)
#define CPU_FEATURE_AVX    (1 << 5)
#define CPU_FEATURE_VMX    (1 << 6)

/* BIOS Information */
struct BIOSInfo {
    char vendor[64];
    char version[64];
    char release_date[64];
    uint32_t revision;
};

/* CPU Information */
struct CPUInfo {
    char vendor[13];           /* CPU Vendor string */
    char brand[49];           /* CPU Brand string */
    uint32_t family;          /* CPU Family */
    uint32_t model;           /* CPU Model */
    uint32_t stepping;        /* CPU Stepping */
    uint32_t max_cpuid;       /* Maximum CPUID level */
    uint32_t max_extended;    /* Maximum extended CPUID level */
    uint64_t features;        /* CPU Features bitfield */
    uint32_t cache_size;      /* CPU Cache size in KB */
    uint32_t cores;           /* Number of CPU cores */
    uint32_t threads;         /* Number of CPU threads */
    uint32_t clock_speed;     /* CPU Clock speed in MHz */
};

/* Memory Information */
struct MemoryInfo {
    uint64_t total_ram;       /* Total RAM in bytes */
    uint64_t available_ram;   /* Available RAM in bytes */
    uint64_t used_ram;        /* Used RAM in bytes */
    uint32_t memory_regions;  /* Number of memory regions */
};

/* System Board Information */
struct SystemBoardInfo {
    char manufacturer[64];
    char product[64];
    char version[64];
    char serial[64];
    char asset_tag[64];
};

/* Boot Configuration */
struct BootConfig {
    uint32_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_bpp;
    uint32_t boot_device;
    uint32_t cmdline_addr;
    uint32_t modules_count;
    uint32_t modules_addr;
    uint8_t boot_flags;
};

/* Combined System Information */
struct SystemInfo {
    const char* os_name;
    const char* version;
    const char* build_date;
    uint32_t uptime_seconds;
    struct BIOSInfo bios;
    struct CPUInfo cpu;
    struct MemoryInfo memory;
    struct SystemBoardInfo board;
    struct BootConfig boot;
};

/* Function declarations */
void sysinfo_init(void);                          /* Initialize system information */
void sysinfo_detect_cpu(struct CPUInfo* cpu);     /* Detect CPU information */
void sysinfo_detect_memory(struct MemoryInfo* mem); /* Detect memory information */
void sysinfo_detect_bios(struct BIOSInfo* bios);  /* Detect BIOS information */
void sysinfo_detect_board(struct SystemBoardInfo* board); /* Detect system board info */
void sysinfo_get_boot_config(struct BootConfig* config);  /* Get boot configuration */
void sysinfo_print_all(void);                     /* Print all system information */ 