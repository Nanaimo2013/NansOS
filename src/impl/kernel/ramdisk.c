/**
 * RAM Disk Filesystem Driver Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "ramdisk.h"
#include <string.h>
#include <stdlib.h>
#include "../../intf/print.h"

/* Static filesystem operations structure */
static struct FSOperations ramdisk_ops = {
    .mount = NULL,
    .unmount = NULL,
    .read = ramdisk_read,
    .write = ramdisk_write,
    .open = NULL,
    .close = NULL,
    .create = ramdisk_create_file,
    .unlink = ramdisk_delete_file,
    .mkdir = NULL,
    .rmdir = NULL,
    .finddir = NULL,
    .readdir = NULL,
    .stat = NULL,
    .chmod = NULL,
    .chown = NULL
};

/* Create a new RAM disk filesystem */
struct FileSystem* ramdisk_create(void) {
    struct FileSystem* fs = (struct FileSystem*)malloc(sizeof(struct FileSystem));
    if (!fs) {
        print_str("[RAMDISK] Error: Failed to allocate filesystem structure\n");
        return NULL;
    }

    struct RamDisk* ramdisk = (struct RamDisk*)malloc(sizeof(struct RamDisk));
    if (!ramdisk) {
        free(fs);
        print_str("[RAMDISK] Error: Failed to allocate RAM disk structure\n");
        return NULL;
    }

    ramdisk->data = (uint8_t*)malloc(RAMDISK_SIZE);
    if (!ramdisk->data) {
        free(ramdisk);
        free(fs);
        print_str("[RAMDISK] Error: Failed to allocate RAM disk data\n");
        return NULL;
    }

    memset(ramdisk->data, 0, RAMDISK_SIZE);
    memset(ramdisk->files, 0, sizeof(ramdisk->files));
    
    ramdisk->total_blocks = RAMDISK_SIZE / RAMDISK_BLOCK_SIZE;
    ramdisk->free_blocks = ramdisk->total_blocks;
    ramdisk->free_block_start = 0;

    strcpy(fs->name, "ramdisk");
    fs->flags = 0;
    fs->device = ramdisk;
    fs->ops = &ramdisk_ops;

    print_str("[RAMDISK] Created RAM disk filesystem (");
    // TODO: Add integer to string conversion
    print_str(" bytes)\n");

    return fs;
}

/* Destroy a RAM disk filesystem */
void ramdisk_destroy(struct FileSystem* fs) {
    if (!fs || !fs->device) {
        return;
    }

    struct RamDisk* ramdisk = (struct RamDisk*)fs->device;
    free(ramdisk->data);
    free(ramdisk);
    free(fs);

    print_str("[RAMDISK] Destroyed RAM disk filesystem\n");
}

/* Find a free file entry */
static struct RamDiskFile* find_free_file(struct RamDisk* ramdisk) {
    for (int i = 0; i < RAMDISK_MAX_FILES; i++) {
        if (!ramdisk->files[i].in_use) {
            return &ramdisk->files[i];
        }
    }
    return NULL;
}

/* Find a file by name */
static struct RamDiskFile* find_file_by_name(struct RamDisk* ramdisk, const char* name) {
    for (int i = 0; i < RAMDISK_MAX_FILES; i++) {
        if (ramdisk->files[i].in_use && strcmp(ramdisk->files[i].name, name) == 0) {
            return &ramdisk->files[i];
        }
    }
    return NULL;
}

/* Allocate blocks for a file */
static int allocate_blocks(struct RamDisk* ramdisk, uint32_t count, uint32_t* start) {
    if (count > ramdisk->free_blocks) {
        return -1;
    }

    *start = ramdisk->free_block_start;
    ramdisk->free_block_start += count;
    ramdisk->free_blocks -= count;
    return 0;
}

/* Create a new file */
int ramdisk_create_file(struct FSNode* parent, const char* name, uint32_t flags) {
    struct FileSystem* fs = parent->fs;
    struct RamDisk* ramdisk = (struct RamDisk*)fs->device;

    if (strlen(name) >= RAMDISK_MAX_NAME_LEN) {
        print_str("[RAMDISK] Error: File name too long\n");
        return -1;
    }

    if (find_file_by_name(ramdisk, name)) {
        print_str("[RAMDISK] Error: File already exists\n");
        return -1;
    }

    struct RamDiskFile* file = find_free_file(ramdisk);
    if (!file) {
        print_str("[RAMDISK] Error: Maximum number of files reached\n");
        return -1;
    }

    strcpy(file->name, name);
    file->flags = flags;
    file->size = 0;
    file->block_start = 0;
    file->block_count = 0;
    file->create_time = 0; // TODO: Add system time
    file->modify_time = file->create_time;
    file->access_time = file->create_time;
    file->in_use = 1;

    print_str("[RAMDISK] Created file: ");
    print_str(name);
    print_str("\n");

    return 0;
}

/* Delete a file */
int ramdisk_delete_file(struct FSNode* node) {
    struct FileSystem* fs = node->fs;
    struct RamDisk* ramdisk = (struct RamDisk*)fs->device;
    struct RamDiskFile* file = find_file_by_name(ramdisk, node->name);

    if (!file) {
        print_str("[RAMDISK] Error: File not found\n");
        return -1;
    }

    // Mark blocks as free
    ramdisk->free_blocks += file->block_count;
    
    // Clear file entry
    memset(file, 0, sizeof(struct RamDiskFile));

    print_str("[RAMDISK] Deleted file: ");
    print_str(node->name);
    print_str("\n");

    return 0;
}

/* Read from a file */
int ramdisk_read(struct FSNode* node, uint64_t offset, uint64_t size, void* buffer) {
    struct FileSystem* fs = node->fs;
    struct RamDisk* ramdisk = (struct RamDisk*)fs->device;
    struct RamDiskFile* file = find_file_by_name(ramdisk, node->name);

    if (!file) {
        print_str("[RAMDISK] Error: File not found\n");
        return -1;
    }

    if (offset >= file->size) {
        return 0;
    }

    uint64_t read_size = size;
    if (offset + size > file->size) {
        read_size = file->size - offset;
    }

    uint8_t* src = ramdisk->data + (file->block_start * RAMDISK_BLOCK_SIZE) + offset;
    memcpy(buffer, src, read_size);
    file->access_time = 0; // TODO: Add system time

    return read_size;
}

/* Write to a file */
int ramdisk_write(struct FSNode* node, uint64_t offset, uint64_t size, const void* buffer) {
    struct FileSystem* fs = node->fs;
    struct RamDisk* ramdisk = (struct RamDisk*)fs->device;
    struct RamDiskFile* file = find_file_by_name(ramdisk, node->name);

    if (!file) {
        print_str("[RAMDISK] Error: File not found\n");
        return -1;
    }

    uint32_t required_blocks = (offset + size + RAMDISK_BLOCK_SIZE - 1) / RAMDISK_BLOCK_SIZE;
    
    if (required_blocks > file->block_count) {
        uint32_t new_blocks = required_blocks - file->block_count;
        uint32_t new_start;
        
        if (allocate_blocks(ramdisk, new_blocks, &new_start) < 0) {
            print_str("[RAMDISK] Error: No space available\n");
            return -1;
        }

        if (file->block_count > 0) {
            // Copy existing data to new location
            memcpy(ramdisk->data + (new_start * RAMDISK_BLOCK_SIZE),
                   ramdisk->data + (file->block_start * RAMDISK_BLOCK_SIZE),
                   file->size);
        }

        file->block_start = new_start;
        file->block_count = required_blocks;
    }

    uint8_t* dest = ramdisk->data + (file->block_start * RAMDISK_BLOCK_SIZE) + offset;
    memcpy(dest, buffer, size);

    if (offset + size > file->size) {
        file->size = offset + size;
    }

    file->modify_time = 0; // TODO: Add system time
    file->access_time = file->modify_time;

    return size;
}

/* Find a file node */
struct FSNode* ramdisk_find_file(struct FileSystem* fs, const char* name) {
    struct RamDisk* ramdisk = (struct RamDisk*)fs->device;
    struct RamDiskFile* file = find_file_by_name(ramdisk, name);

    if (!file) {
        return NULL;
    }

    struct FSNode* node = (struct FSNode*)malloc(sizeof(struct FSNode));
    if (!node) {
        return NULL;
    }

    strcpy(node->name, file->name);
    node->flags = file->flags;
    node->permissions = FS_PERM_READ | FS_PERM_WRITE;
    node->uid = 0;
    node->gid = 0;
    node->size = file->size;
    node->inode = 0;
    node->atime = file->access_time;
    node->mtime = file->modify_time;
    node->ctime = file->create_time;
    node->fs = fs;
    node->private_data = file;

    return node;
} 