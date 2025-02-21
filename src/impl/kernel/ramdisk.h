/**
 * RAM Disk Filesystem Driver
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include "fs.h"

/* RAM disk configuration */
#define RAMDISK_SIZE (1024 * 1024)  // 1MB RAM disk
#define RAMDISK_BLOCK_SIZE 512
#define RAMDISK_MAX_FILES 64
#define RAMDISK_MAX_NAME_LEN 32

/* RAM disk file entry */
struct RamDiskFile {
    char name[RAMDISK_MAX_NAME_LEN];
    uint32_t flags;
    uint32_t size;
    uint32_t block_start;
    uint32_t block_count;
    uint64_t create_time;
    uint64_t modify_time;
    uint64_t access_time;
    int in_use;
};

/* RAM disk structure */
struct RamDisk {
    uint8_t* data;
    struct RamDiskFile files[RAMDISK_MAX_FILES];
    uint32_t free_block_start;
    uint32_t total_blocks;
    uint32_t free_blocks;
};

/* RAM disk operations */
struct FileSystem* ramdisk_create(void);
void ramdisk_destroy(struct FileSystem* fs);
int ramdisk_read(struct FSNode* node, uint64_t offset, uint64_t size, void* buffer);
int ramdisk_write(struct FSNode* node, uint64_t offset, uint64_t size, const void* buffer);
int ramdisk_create_file(struct FSNode* parent, const char* name, uint32_t flags);
int ramdisk_delete_file(struct FSNode* node);
struct FSNode* ramdisk_find_file(struct FileSystem* fs, const char* name); 