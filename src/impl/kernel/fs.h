/**
 * Virtual File System
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* File types */
#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

/* File permissions */
#define FS_PERM_READ    0x04
#define FS_PERM_WRITE   0x02
#define FS_PERM_EXEC    0x01

/* File flags */
#define FS_O_RDONLY     0x0000
#define FS_O_WRONLY     0x0001
#define FS_O_RDWR       0x0002
#define FS_O_APPEND     0x0008
#define FS_O_CREAT      0x0200
#define FS_O_TRUNC      0x0400
#define FS_O_EXCL       0x0800

/* Seek modes */
#define FS_SEEK_SET     0
#define FS_SEEK_CUR     1
#define FS_SEEK_END     2

/* Maximum path length */
#define FS_MAX_PATH     256

/* File system structures */
struct FileSystem {
    char name[32];
    uint32_t flags;
    void* device;
    struct FSOperations* ops;
};

struct FSNode {
    char name[FS_MAX_PATH];
    uint32_t flags;
    uint32_t permissions;
    uint32_t uid;
    uint32_t gid;
    uint64_t size;
    uint64_t inode;
    uint64_t atime;
    uint64_t mtime;
    uint64_t ctime;
    struct FileSystem* fs;
    void* private_data;
};

struct FSOperations {
    int (*mount)(struct FileSystem* fs, const char* device);
    int (*unmount)(struct FileSystem* fs);
    int (*read)(struct FSNode* node, uint64_t offset, uint64_t size, void* buffer);
    int (*write)(struct FSNode* node, uint64_t offset, uint64_t size, const void* buffer);
    int (*open)(struct FSNode* node, uint32_t flags);
    int (*close)(struct FSNode* node);
    int (*create)(struct FSNode* parent, const char* name, uint32_t flags);
    int (*unlink)(struct FSNode* node);
    int (*mkdir)(struct FSNode* parent, const char* name, uint32_t flags);
    int (*rmdir)(struct FSNode* node);
    struct FSNode* (*finddir)(struct FSNode* node, const char* name);
    int (*readdir)(struct FSNode* node, uint32_t index, struct FSNode* out);
    int (*stat)(struct FSNode* node, struct FSNode* out);
    int (*chmod)(struct FSNode* node, uint32_t mode);
    int (*chown)(struct FSNode* node, uint32_t uid, uint32_t gid);
};

/* File descriptor */
struct FileDescriptor {
    struct FSNode* node;
    uint64_t offset;
    uint32_t flags;
    uint32_t refcount;
};

/* VFS functions */
void vfs_init(void);
int vfs_mount(const char* path, const char* device, const char* fs_type);
int vfs_unmount(const char* path);
struct FSNode* vfs_lookup(const char* path);
int vfs_open(const char* path, uint32_t flags);
int vfs_close(int fd);
int64_t vfs_read(int fd, void* buffer, uint64_t size);
int64_t vfs_write(int fd, const void* buffer, uint64_t size);
int64_t vfs_seek(int fd, int64_t offset, int whence);
int vfs_stat(const char* path, struct FSNode* stat);
int vfs_mkdir(const char* path, uint32_t mode);
int vfs_rmdir(const char* path);
int vfs_unlink(const char* path);
int vfs_rename(const char* oldpath, const char* newpath);
int vfs_chmod(const char* path, uint32_t mode);
int vfs_chown(const char* path, uint32_t uid, uint32_t gid);

/* Directory operations */
struct FSNode* vfs_opendir(const char* path);
int vfs_closedir(struct FSNode* dir);
struct FSNode* vfs_readdir(struct FSNode* dir);
void vfs_rewinddir(struct FSNode* dir);

/* Path operations */
char* vfs_normalize_path(const char* path);
char* vfs_absolute_path(const char* path);
int vfs_is_directory(const char* path);
int vfs_is_file(const char* path);
int vfs_exists(const char* path); 