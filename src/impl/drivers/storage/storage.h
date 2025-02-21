/**
 * Storage Driver Interface
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* Storage device types */
#define STORAGE_TYPE_UNKNOWN   0
#define STORAGE_TYPE_IDE      1
#define STORAGE_TYPE_AHCI     2
#define STORAGE_TYPE_NVME     3
#define STORAGE_TYPE_RAMDISK  4

/* Storage device flags */
#define STORAGE_FLAG_REMOVABLE 0x01
#define STORAGE_FLAG_READONLY  0x02
#define STORAGE_FLAG_BOOT     0x04

/* Storage device information */
struct StorageDeviceInfo {
    char name[32];
    uint32_t type;
    uint32_t flags;
    uint64_t total_sectors;
    uint32_t sector_size;
    uint32_t max_transfer;
    char model[41];
    char serial[21];
    char firmware[9];
};

/* Storage device operations */
struct StorageDeviceOps {
    int (*init)(struct StorageDevice* dev);
    int (*read_sectors)(struct StorageDevice* dev, uint64_t sector, uint32_t count, void* buffer);
    int (*write_sectors)(struct StorageDevice* dev, uint64_t sector, uint32_t count, const void* buffer);
    int (*flush)(struct StorageDevice* dev);
    void (*cleanup)(struct StorageDevice* dev);
};

/* Storage device structure */
struct StorageDevice {
    struct StorageDeviceInfo info;
    struct StorageDeviceOps* ops;
    void* private_data;
};

/* Storage manager functions */
void storage_init(void);
struct StorageDevice* storage_probe_devices(void);
int storage_register_device(struct StorageDevice* dev);
void storage_unregister_device(struct StorageDevice* dev);
struct StorageDevice* storage_get_device(const char* name);
int storage_get_device_list(struct StorageDeviceInfo* list, int max_devices);

/* Helper functions */
const char* storage_type_to_string(uint32_t type);
void storage_print_device_info(struct StorageDeviceInfo* info); 