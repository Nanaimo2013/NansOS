/**
 * IDE Storage Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include "storage.h"

/* IDE controller ports */
#define IDE_PRIMARY_DATA        0x1F0
#define IDE_PRIMARY_ERROR      0x1F1
#define IDE_PRIMARY_SECCOUNT   0x1F2
#define IDE_PRIMARY_LBA_LOW    0x1F3
#define IDE_PRIMARY_LBA_MID    0x1F4
#define IDE_PRIMARY_LBA_HIGH   0x1F5
#define IDE_PRIMARY_DRIVE      0x1F6
#define IDE_PRIMARY_STATUS     0x1F7
#define IDE_PRIMARY_COMMAND    0x1F7

#define IDE_SECONDARY_DATA     0x170
#define IDE_SECONDARY_ERROR    0x171
#define IDE_SECONDARY_SECCOUNT 0x172
#define IDE_SECONDARY_LBA_LOW  0x173
#define IDE_SECONDARY_LBA_MID  0x174
#define IDE_SECONDARY_LBA_HIGH 0x175
#define IDE_SECONDARY_DRIVE    0x176
#define IDE_SECONDARY_STATUS   0x177
#define IDE_SECONDARY_COMMAND  0x177

/* IDE status register bits */
#define IDE_SR_ERR  0x01  /* Error */
#define IDE_SR_IDX  0x02  /* Index */
#define IDE_SR_CORR 0x04  /* Corrected data */
#define IDE_SR_DRQ  0x08  /* Data request ready */
#define IDE_SR_DSC  0x10  /* Drive seek complete */
#define IDE_SR_DWF  0x20  /* Drive write fault */
#define IDE_SR_DRDY 0x40  /* Drive ready */
#define IDE_SR_BSY  0x80  /* Busy */

/* IDE commands */
#define IDE_CMD_READ_PIO        0x20
#define IDE_CMD_READ_PIO_EXT    0x24
#define IDE_CMD_WRITE_PIO       0x30
#define IDE_CMD_WRITE_PIO_EXT   0x34
#define IDE_CMD_CACHE_FLUSH     0xE7
#define IDE_CMD_CACHE_FLUSH_EXT 0xEA
#define IDE_CMD_PACKET          0xA0
#define IDE_CMD_IDENTIFY_PACKET 0xA1
#define IDE_CMD_IDENTIFY        0xEC

/* IDE identification space */
#define IDE_IDENT_DEVICETYPE   0
#define IDE_IDENT_CYLINDERS    2
#define IDE_IDENT_HEADS        6
#define IDE_IDENT_SECTORS      12
#define IDE_IDENT_SERIAL       20
#define IDE_IDENT_MODEL        54
#define IDE_IDENT_CAPABILITIES 98
#define IDE_IDENT_FIELDVALID   106
#define IDE_IDENT_MAX_LBA      120
#define IDE_IDENT_COMMANDSETS  164
#define IDE_IDENT_MAX_LBA_EXT  200

/* IDE device structure */
struct IDEDevice {
    uint16_t base;          /* I/O base port */
    uint16_t ctrl;          /* Control port */
    uint16_t bmide;         /* Bus master IDE port */
    uint8_t slave;          /* Master/slave flag */
    uint8_t use_lba48;      /* LBA48 support flag */
    uint16_t signature;     /* Drive signature */
    uint16_t capabilities;  /* Features supported */
    uint32_t commandsets;   /* Command sets supported */
    uint32_t size;          /* Size in sectors */
};

/* IDE driver functions */
struct StorageDevice* ide_init_device(uint16_t base, uint8_t slave);
void ide_cleanup_device(struct StorageDevice* dev);

/* Internal functions (exposed for testing) */
int ide_identify(struct IDEDevice* dev);
int ide_wait_ready(uint16_t base);
int ide_wait_busy(uint16_t base);
void ide_select_drive(struct IDEDevice* dev);
int ide_read_sectors(struct IDEDevice* dev, uint64_t lba, uint8_t sectors, void* buffer);
int ide_write_sectors(struct IDEDevice* dev, uint64_t lba, uint8_t sectors, const void* buffer); 