/**
 * Storage Driver Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "storage.h"
#include "../serial/serial.h"
#include "../../kernel/asm_utils.h"
#include <string.h>
#include <stdio.h>

/* Maximum number of storage devices */
#define MAX_STORAGE_DEVICES 16

/* Storage device registry */
static struct StorageDevice* devices[MAX_STORAGE_DEVICES];
static int num_devices = 0;

/* Initialize storage subsystem */
void storage_init(void) {
    serial_write_string(COM1_PORT, "[STORAGE] Initializing storage subsystem...\n");
    
    /* Clear device registry */
    memset(devices, 0, sizeof(devices));
    num_devices = 0;
    
    serial_write_string(COM1_PORT, "[STORAGE] Storage subsystem initialized\n");
}

/* Register a storage device */
int storage_register_device(struct StorageDevice* dev) {
    if (!dev) {
        serial_write_string(COM1_PORT, "[STORAGE] Error: Cannot register NULL device\n");
        return -1;
    }
    
    if (num_devices >= MAX_STORAGE_DEVICES) {
        serial_write_string(COM1_PORT, "[STORAGE] Error: Maximum number of devices reached\n");
        return -1;
    }
    
    /* Initialize the device */
    if (dev->ops && dev->ops->init) {
        int result = dev->ops->init(dev);
        if (result != 0) {
            char error_msg[64];
            sprintf(error_msg, "[STORAGE] Error: Device initialization failed: %d\n", result);
            serial_write_string(COM1_PORT, error_msg);
            return result;
        }
    }
    
    /* Add to registry */
    devices[num_devices++] = dev;
    
    /* Print device info */
    char info_msg[128];
    sprintf(info_msg, "[STORAGE] Registered device: %s (%s)\n", 
            dev->info.name, storage_type_to_string(dev->info.type));
    serial_write_string(COM1_PORT, info_msg);
    
    return 0;
}

/* Unregister a storage device */
void storage_unregister_device(struct StorageDevice* dev) {
    if (!dev) return;
    
    /* Find device in registry */
    for (int i = 0; i < num_devices; i++) {
        if (devices[i] == dev) {
            /* Call cleanup if available */
            if (dev->ops && dev->ops->cleanup) {
                dev->ops->cleanup(dev);
            }
            
            /* Remove from registry */
            for (int j = i; j < num_devices - 1; j++) {
                devices[j] = devices[j + 1];
            }
            devices[--num_devices] = NULL;
            
            char info_msg[64];
            sprintf(info_msg, "[STORAGE] Unregistered device: %s\n", dev->info.name);
            serial_write_string(COM1_PORT, info_msg);
            return;
        }
    }
}

/* Get device by name */
struct StorageDevice* storage_get_device(const char* name) {
    if (!name) return NULL;
    
    for (int i = 0; i < num_devices; i++) {
        if (devices[i] && strcmp(devices[i]->info.name, name) == 0) {
            return devices[i];
        }
    }
    
    return NULL;
}

/* Get list of storage devices */
int storage_get_device_list(struct StorageDeviceInfo* list, int max_devices) {
    if (!list || max_devices <= 0) return 0;
    
    int count = (num_devices < max_devices) ? num_devices : max_devices;
    
    for (int i = 0; i < count; i++) {
        memcpy(&list[i], &devices[i]->info, sizeof(struct StorageDeviceInfo));
    }
    
    return count;
}

/* Convert storage type to string */
const char* storage_type_to_string(uint32_t type) {
    switch (type) {
        case STORAGE_TYPE_IDE:
            return "IDE";
        case STORAGE_TYPE_AHCI:
            return "AHCI";
        case STORAGE_TYPE_NVME:
            return "NVMe";
        case STORAGE_TYPE_RAMDISK:
            return "RAM Disk";
        default:
            return "Unknown";
    }
}

/* Print storage device information */
void storage_print_device_info(struct StorageDeviceInfo* info) {
    if (!info) return;
    
    char buffer[256];
    sprintf(buffer,
            "Storage Device Information:\n"
            "  Name: %s\n"
            "  Type: %s\n"
            "  Flags: %s%s%s\n"
            "  Capacity: %llu sectors (%llu bytes)\n"
            "  Sector Size: %u bytes\n"
            "  Model: %s\n"
            "  Serial: %s\n"
            "  Firmware: %s\n",
            info->name,
            storage_type_to_string(info->type),
            (info->flags & STORAGE_FLAG_REMOVABLE) ? "Removable " : "",
            (info->flags & STORAGE_FLAG_READONLY) ? "ReadOnly " : "",
            (info->flags & STORAGE_FLAG_BOOT) ? "Boot " : "",
            info->total_sectors,
            info->total_sectors * info->sector_size,
            info->sector_size,
            info->model,
            info->serial,
            info->firmware);
            
    serial_write_string(COM1_PORT, buffer);
}

/* Probe for storage devices */
struct StorageDevice* storage_probe_devices(void) {
    serial_write_string(COM1_PORT, "[STORAGE] Probing for storage devices...\n");
    
    /* TODO: Implement probing for different storage types */
    /* This will be implemented in separate driver files */
    
    char info_msg[64];
    sprintf(info_msg, "[STORAGE] Found %d storage devices\n", num_devices);
    serial_write_string(COM1_PORT, info_msg);
    
    return devices[0]; /* Return first device (usually boot device) */
} 