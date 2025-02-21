/**
 * System Information Gathering Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "sysinfo.h"
#include "asm_utils.h"
#include "../../intf/print.h"
#include "../drivers/serial/serial.h"
#include <string.h>

/* Global system information structure */
static struct SystemInfo sys_info;

/* CPUID function */
static void cpuid(uint32_t leaf, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    asm_cpuid(leaf, eax, ebx, ecx, edx);
}

/* Read MSR */
static uint64_t read_msr(uint32_t msr) {
    return asm_rdmsr(msr);
}

void sysinfo_detect_cpu(struct CPUInfo* cpu) {
    uint32_t eax, ebx, ecx, edx;
    
    /* Get vendor string */
    cpuid(0, &cpu->max_cpuid, (uint32_t*)&cpu->vendor[0],
          (uint32_t*)&cpu->vendor[8], (uint32_t*)&cpu->vendor[4]);
    cpu->vendor[12] = '\0';

    /* Get brand string */
    if (cpu->max_cpuid >= 0x80000004) {
        for (int i = 0; i < 3; i++) {
            cpuid(0x80000002 + i, (uint32_t*)&cpu->brand[i * 16],
                  (uint32_t*)&cpu->brand[i * 16 + 4],
                  (uint32_t*)&cpu->brand[i * 16 + 8],
                  (uint32_t*)&cpu->brand[i * 16 + 12]);
        }
        cpu->brand[48] = '\0';
    } else {
        strcpy(cpu->brand, "Unknown");
    }

    /* Get CPU features */
    cpuid(1, &eax, &ebx, &ecx, &edx);
    cpu->family = (eax >> 8) & 0xF;
    cpu->model = (eax >> 4) & 0xF;
    cpu->stepping = eax & 0xF;
    
    /* Set feature flags */
    cpu->features = 0;
    if (edx & (1 << 0)) cpu->features |= CPU_FEATURE_FPU;
    if (edx & (1 << 23)) cpu->features |= CPU_FEATURE_MMX;
    if (edx & (1 << 25)) cpu->features |= CPU_FEATURE_SSE;
    if (edx & (1 << 26)) cpu->features |= CPU_FEATURE_SSE2;
    if (ecx & (1 << 0)) cpu->features |= CPU_FEATURE_SSE3;
    if (ecx & (1 << 28)) cpu->features |= CPU_FEATURE_AVX;
    if (ecx & (1 << 5)) cpu->features |= CPU_FEATURE_VMX;

    /* Get core and thread count */
    cpuid(1, &eax, &ebx, &ecx, &edx);
    cpu->threads = (ebx >> 16) & 0xFF;
    cpu->cores = cpu->threads / 2;  /* Assuming Hyperthreading */

    /* Get cache size */
    cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
    cpu->cache_size = (ecx >> 16) & 0xFFFF;  /* L2 cache size in KB */

    /* Estimate clock speed (this is just a rough estimate) */
    cpu->clock_speed = 1000;  /* Placeholder: 1GHz */
}

void sysinfo_detect_memory(struct MemoryInfo* mem) {
    /* This would normally use BIOS/UEFI calls or parse multiboot info */
    /* For now, we'll use placeholder values */
    mem->total_ram = 256 * 1024 * 1024;      /* 256 MB */
    mem->available_ram = 240 * 1024 * 1024;   /* 240 MB */
    mem->used_ram = 16 * 1024 * 1024;        /* 16 MB */
    mem->memory_regions = 1;
}

void sysinfo_detect_bios(struct BIOSInfo* bios) {
    /* This would normally read from BIOS memory or use UEFI services */
    strcpy(bios->vendor, "NansOS BIOS");
    strcpy(bios->version, "1.0.0");
    strcpy(bios->release_date, __DATE__);
    bios->revision = 1;
}

void sysinfo_detect_board(struct SystemBoardInfo* board) {
    /* This would normally read from SMBIOS/DMI */
    strcpy(board->manufacturer, "NansOS Virtual Systems");
    strcpy(board->product, "NansOS Development Board");
    strcpy(board->version, "1.0");
    strcpy(board->serial, "NANS0001");
    strcpy(board->asset_tag, "NANS-DEV-001");
}

void sysinfo_get_boot_config(struct BootConfig* config) {
    /* This would normally parse multiboot information */
    config->framebuffer_width = 1024;
    config->framebuffer_height = 768;
    config->framebuffer_bpp = 32;
    config->framebuffer_pitch = config->framebuffer_width * 4;
    config->boot_flags = 0;
}

void sysinfo_init(void) {
    /* Initialize basic information */
    sys_info.os_name = "NansOS";
    sys_info.version = "0.1.0";
    sys_info.build_date = __DATE__;
    sys_info.uptime_seconds = 0;

    /* Detect all hardware information */
    sysinfo_detect_cpu(&sys_info.cpu);
    sysinfo_detect_memory(&sys_info.memory);
    sysinfo_detect_bios(&sys_info.bios);
    sysinfo_detect_board(&sys_info.board);
    sysinfo_get_boot_config(&sys_info.boot);
}

void sysinfo_print_all(void) {
    char str[256];

    print_str("\n=== System Information ===\n");
    
    /* Print OS info */
    sprintf(str, "OS: %s v%s (Built: %s)\n",
            sys_info.os_name, sys_info.version, sys_info.build_date);
    print_str(str);

    /* Print CPU info */
    print_str("\n--- CPU Information ---\n");
    sprintf(str, "Vendor: %s\n", sys_info.cpu.vendor);
    print_str(str);
    sprintf(str, "Brand: %s\n", sys_info.cpu.brand);
    print_str(str);
    sprintf(str, "Family: %d, Model: %d, Stepping: %d\n",
            sys_info.cpu.family, sys_info.cpu.model, sys_info.cpu.stepping);
    print_str(str);
    sprintf(str, "Cores: %d, Threads: %d\n",
            sys_info.cpu.cores, sys_info.cpu.threads);
    print_str(str);
    sprintf(str, "Cache: %dKB, Clock: %dMHz\n",
            sys_info.cpu.cache_size, sys_info.cpu.clock_speed);
    print_str(str);

    /* Print memory info */
    print_str("\n--- Memory Information ---\n");
    sprintf(str, "Total RAM: %dMB\n", (uint32_t)(sys_info.memory.total_ram / 1024 / 1024));
    print_str(str);
    sprintf(str, "Available: %dMB\n", (uint32_t)(sys_info.memory.available_ram / 1024 / 1024));
    print_str(str);
    sprintf(str, "Used: %dMB\n", (uint32_t)(sys_info.memory.used_ram / 1024 / 1024));
    print_str(str);

    /* Print BIOS info */
    print_str("\n--- BIOS Information ---\n");
    sprintf(str, "Vendor: %s\n", sys_info.bios.vendor);
    print_str(str);
    sprintf(str, "Version: %s (%s)\n", sys_info.bios.version, sys_info.bios.release_date);
    print_str(str);

    /* Print board info */
    print_str("\n--- System Board ---\n");
    sprintf(str, "Manufacturer: %s\n", sys_info.board.manufacturer);
    print_str(str);
    sprintf(str, "Product: %s\n", sys_info.board.product);
    print_str(str);
    sprintf(str, "Version: %s\n", sys_info.board.version);
    print_str(str);

    /* Print boot config */
    print_str("\n--- Boot Configuration ---\n");
    sprintf(str, "Display: %dx%dx%d\n",
            sys_info.boot.framebuffer_width,
            sys_info.boot.framebuffer_height,
            sys_info.boot.framebuffer_bpp);
    print_str(str);

    print_str("\n=========================\n");
} 