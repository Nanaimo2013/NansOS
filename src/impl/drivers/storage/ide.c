/**
 * IDE Storage Driver Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "ide.h"
#include "../port_io/port.h"
#include "../serial/serial.h"
#include "../../kernel/asm_utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Maximum number of sectors to transfer at once */
#define MAX_SECTORS_PER_TRANSFER 256

/* Static operations structure */
static struct StorageDeviceOps ide_ops = {
    .init = NULL,
    .read_sectors = (int (*)(struct StorageDevice*, uint64_t, uint32_t, void*))ide_read_sectors,
    .write_sectors = (int (*)(struct StorageDevice*, uint64_t, uint32_t, const void*))ide_write_sectors,
    .flush = NULL,
    .cleanup = (void (*)(struct StorageDevice*))ide_cleanup_device
};

/* Wait for IDE controller to be ready */
int ide_wait_ready(uint16_t base) {
    uint8_t status;
    int timeout = 100000;  /* Arbitrary timeout value */
    
    while (--timeout > 0) {
        status = port_byte_in(base + 7);
        if (!(status & IDE_SR_BSY)) {
            if (status & IDE_SR_DRDY) {
                return 0;  /* Success */
            }
        }
        port_io_wait();  /* Small delay */
    }
    
    return -1;  /* Timeout */
}

/* Wait for IDE controller to not be busy */
int ide_wait_busy(uint16_t base) {
    uint8_t status;
    int timeout = 100000;  /* Arbitrary timeout value */
    
    while (--timeout > 0) {
        status = port_byte_in(base + 7);
        if (!(status & IDE_SR_BSY)) {
            return 0;  /* Success */
        }
        port_io_wait();  /* Small delay */
    }
    
    return -1;  /* Timeout */
}

/* Select IDE drive */
void ide_select_drive(struct IDEDevice* dev) {
    port_byte_out(dev->base + 6, 0xA0 | (dev->slave << 4));
    port_io_wait();  /* Small delay */
}

/* Read IDE identification space */
static void ide_read_identify_space(uint16_t* buf, int offset, int length, char* dest) {
    /* Copy and swap bytes */
    for (int i = 0; i < length/2; i++) {
        dest[i*2] = (buf[offset + i] >> 8) & 0xFF;
        dest[i*2 + 1] = buf[offset + i] & 0xFF;
    }
    dest[length] = '\0';
    
    /* Trim trailing spaces */
    for (int i = length-1; i >= 0; i--) {
        if (dest[i] == ' ') {
            dest[i] = '\0';
        } else {
            break;
        }
    }
}

/* Identify IDE device */
int ide_identify(struct IDEDevice* dev) {
    uint16_t identify_data[256] = {0};  // Initialize array to zero
    int timeout = 100000;
    
    /* Select drive */
    ide_select_drive(dev);
    
    /* Send identify command */
    port_byte_out(dev->base + IDE_PRIMARY_SECCOUNT, 0);
    port_byte_out(dev->base + IDE_PRIMARY_LBA_LOW, 0);
    port_byte_out(dev->base + IDE_PRIMARY_LBA_MID, 0);
    port_byte_out(dev->base + IDE_PRIMARY_LBA_HIGH, 0);
    port_byte_out(dev->base + IDE_PRIMARY_COMMAND, IDE_CMD_IDENTIFY);
    
    /* Wait for data or error */
    while (--timeout > 0) {
        uint8_t status = port_byte_in(dev->base + IDE_PRIMARY_STATUS);
        if (status & IDE_SR_ERR) {
            return -1;  /* Device error */
        }
        if (status & IDE_SR_DRQ) {
            break;  /* Data ready */
        }
    }
    if (timeout == 0) {
        return -1;  /* Timeout */
    }
    
    /* Read identify data */
    for (int i = 0; i < 256; i++) {
        identify_data[i] = port_word_in(dev->base);
    }
    
    /* Extract device information */
    dev->signature = identify_data[IDE_IDENT_DEVICETYPE];
    dev->capabilities = identify_data[IDE_IDENT_CAPABILITIES];
    dev->commandsets = *(uint32_t*)&identify_data[IDE_IDENT_COMMANDSETS];
    
    /* Get size */
    if (dev->commandsets & (1 << 26)) {
        /* Device supports 48-bit LBA */
        dev->size = *(uint32_t*)&identify_data[IDE_IDENT_MAX_LBA_EXT];
        dev->use_lba48 = 1;
    } else {
        /* Use standard 28-bit LBA */
        dev->size = *(uint32_t*)&identify_data[IDE_IDENT_MAX_LBA];
        dev->use_lba48 = 0;
    }
    
    /* Get strings */
    char model[41];
    char serial[21];
    ide_read_identify_space(identify_data, IDE_IDENT_MODEL, 40, model);
    ide_read_identify_space(identify_data, IDE_IDENT_SERIAL, 20, serial);
    
    return 0;
}

/* Read sectors from IDE device */
int ide_read_sectors(struct IDEDevice* dev, uint64_t lba, uint8_t sectors, void* buffer) {
    if (!dev || !buffer || sectors == 0) {
        return -1;
    }
    
    /* Select the drive */
    ide_select_drive(dev);
    
    /* Wait for drive to be ready */
    if (ide_wait_ready(dev->base) < 0) {
        return -1;
    }
    
    if (dev->use_lba48) {
        /* LBA48 mode */
        port_byte_out(dev->base + 2, sectors);
        port_byte_out(dev->base + 3, (uint8_t)(lba));
        port_byte_out(dev->base + 4, (uint8_t)(lba >> 8));
        port_byte_out(dev->base + 5, (uint8_t)(lba >> 16));
        port_byte_out(dev->base + 2, (uint8_t)(lba >> 24));
        port_byte_out(dev->base + 3, (uint8_t)(lba >> 32));
        port_byte_out(dev->base + 4, (uint8_t)(lba >> 40));
        port_byte_out(dev->base + 7, IDE_CMD_READ_PIO_EXT);
    } else {
        /* LBA28 mode */
        port_byte_out(dev->base + 2, sectors);
        port_byte_out(dev->base + 3, (uint8_t)(lba));
        port_byte_out(dev->base + 4, (uint8_t)(lba >> 8));
        port_byte_out(dev->base + 5, (uint8_t)(lba >> 16));
        port_byte_out(dev->base + 6, 0xE0 | (dev->slave << 4) | ((lba >> 24) & 0x0F));
        port_byte_out(dev->base + 7, IDE_CMD_READ_PIO);
    }
    
    /* Read the data */
    uint16_t* buf = (uint16_t*)buffer;
    for (int i = 0; i < sectors; i++) {
        /* Wait for data */
        if (ide_wait_ready(dev->base) < 0) {
            return -1;
        }
        
        /* Read sector */
        for (int j = 0; j < 256; j++) {
            buf[j] = port_word_in(dev->base);
        }
        
        buf += 256;  /* Next sector */
    }
    
    return 0;
}

/* Write sectors to IDE device */
int ide_write_sectors(struct IDEDevice* dev, uint64_t lba, uint8_t sectors, const void* buffer) {
    if (!dev || !buffer || sectors == 0) {
        return -1;
    }
    
    /* Select the drive */
    ide_select_drive(dev);
    
    /* Wait for drive to be ready */
    if (ide_wait_ready(dev->base) < 0) {
        return -1;
    }
    
    if (dev->use_lba48) {
        /* LBA48 mode */
        port_byte_out(dev->base + 2, sectors);
        port_byte_out(dev->base + 3, (uint8_t)(lba));
        port_byte_out(dev->base + 4, (uint8_t)(lba >> 8));
        port_byte_out(dev->base + 5, (uint8_t)(lba >> 16));
        port_byte_out(dev->base + 2, (uint8_t)(lba >> 24));
        port_byte_out(dev->base + 3, (uint8_t)(lba >> 32));
        port_byte_out(dev->base + 4, (uint8_t)(lba >> 40));
        port_byte_out(dev->base + 7, IDE_CMD_WRITE_PIO_EXT);
    } else {
        /* LBA28 mode */
        port_byte_out(dev->base + 2, sectors);
        port_byte_out(dev->base + 3, (uint8_t)(lba));
        port_byte_out(dev->base + 4, (uint8_t)(lba >> 8));
        port_byte_out(dev->base + 5, (uint8_t)(lba >> 16));
        port_byte_out(dev->base + 6, 0xE0 | (dev->slave << 4) | ((lba >> 24) & 0x0F));
        port_byte_out(dev->base + 7, IDE_CMD_WRITE_PIO);
    }
    
    /* Write the data */
    const uint16_t* buf = (const uint16_t*)buffer;
    for (int i = 0; i < sectors; i++) {
        /* Wait for drive ready */
        if (ide_wait_ready(dev->base) < 0) {
            return -1;
        }
        
        /* Write sector */
        for (int j = 0; j < 256; j++) {
            port_word_out(dev->base, buf[j]);
        }
        
        buf += 256;  /* Next sector */
        
        /* Flush cache if this is the last sector */
        if (i == sectors - 1) {
            port_byte_out(dev->base + 7, IDE_CMD_CACHE_FLUSH);
            ide_wait_ready(dev->base);
        }
    }
    
    return 0;
}

/* Initialize IDE device */
struct StorageDevice* ide_init_device(uint16_t base, uint8_t slave) {
    struct StorageDevice* dev = malloc(sizeof(struct StorageDevice));
    if (!dev) {
        serial_write_string(COM1_PORT, "[IDE] Error: Failed to allocate device structure\n");
        return NULL;
    }

    struct IDEDevice* ide_dev = malloc(sizeof(struct IDEDevice));
    if (!ide_dev) {
        free(dev);
        serial_write_string(COM1_PORT, "[IDE] Error: Failed to allocate IDE structure\n");
        return NULL;
    }

    /* Initialize IDE device */
    ide_dev->base = base;
    ide_dev->slave = slave;
    ide_dev->use_lba48 = 0;

    /* Initialize storage device info */
    strcpy(dev->info.name, slave ? "ide1" : "ide0");
    dev->info.type = STORAGE_TYPE_IDE;
    dev->info.flags = 0;
    dev->info.sector_size = 512;
    dev->info.max_transfer = 256;  /* Maximum sectors per transfer */

    /* Identify device */
    if (ide_identify(ide_dev) != 0) {
        free(ide_dev);
        free(dev);
        serial_write_string(COM1_PORT, "[IDE] Error: Device identification failed\n");
        return NULL;
    }

    /* Set up device structure */
    dev->ops = &ide_ops;
    dev->private_data = ide_dev;

    return dev;
}

/* Clean up IDE device */
void ide_cleanup_device(struct StorageDevice* dev) {
    if (!dev) return;
    
    struct IDEDevice* ide_dev = dev->private_data;
    if (ide_dev) {
        free(ide_dev);
    }
    free(dev);
} 