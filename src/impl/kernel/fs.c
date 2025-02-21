/**
 * Virtual File System Implementation
 * NansOS Kernel System
 * Copyright (c) 2025 NansStudios
 */

#include "fs.h"
#include <stddef.h>
#include <string.h>
#include "../../intf/print.h"

#define MAX_FD 256
#define MAX_MOUNTS 16
#define MAX_PATH_LEN 256

/* Global variables */
static struct FileDescriptor fd_table[MAX_FD];
static struct {
    char* path;
    struct FileSystem* fs;
} mount_points[MAX_MOUNTS];

static int num_mounts = 0;
static struct FSNode* root_node = NULL;

/* Normalize a path (remove .., ., and multiple slashes) */
char* vfs_normalize_path(const char* path) {
    if (!path) return NULL;
    
    static char normalized[MAX_PATH_LEN];
    char* np = normalized;
    const char* pp = path;
    
    /* Handle absolute vs relative path */
    if (*pp == '/') {
        *np++ = '/';
        pp++;
    }
    
    while (*pp) {
        /* Skip multiple slashes */
        if (*pp == '/') {
            while (*pp == '/') pp++;
            if (*pp) *np++ = '/';
            continue;
        }
        
        /* Handle . and .. */
        if (*pp == '.') {
            if (pp[1] == '/' || pp[1] == '\0') {
                pp += 1;
                continue;
            }
            if (pp[1] == '.' && (pp[2] == '/' || pp[2] == '\0')) {
                pp += 2;
                if (np > normalized + 1) {
                    np--;
                    while (np > normalized && np[-1] != '/') np--;
                }
                continue;
            }
        }
        
        /* Copy regular path component */
        while (*pp && *pp != '/') {
            *np++ = *pp++;
        }
    }
    
    /* Terminate string */
    if (np == normalized) *np++ = '/';
    *np = '\0';
    
    return normalized;
}

/* Find a filesystem node by path */
struct FSNode* vfs_lookup(const char* path) {
    if (!path) return NULL;
    
    /* Normalize path */
    char* norm_path = vfs_normalize_path(path);
    if (!norm_path) return NULL;
    
    /* Handle root */
    if (strcmp(norm_path, "/") == 0) {
        return root_node;
    }
    
    /* Find mount point */
    struct FileSystem* fs = NULL;
    const char* rel_path = norm_path;
    
    for (int i = 0; i < num_mounts; i++) {
        size_t mlen = strlen(mount_points[i].path);
        if (strncmp(norm_path, mount_points[i].path, mlen) == 0) {
            if (norm_path[mlen] == '/' || norm_path[mlen] == '\0') {
                fs = mount_points[i].fs;
                rel_path = norm_path + mlen;
                while (*rel_path == '/') rel_path++;
                break;
            }
        }
    }
    
    if (!fs) return NULL;
    
    /* Walk the path */
    struct FSNode* node = root_node;
    char component[MAX_PATH_LEN];
    const char* p = rel_path;
    
    while (*p) {
        /* Extract next path component */
        char* cp = component;
        while (*p && *p != '/') {
            *cp++ = *p++;
        }
        *cp = '\0';
        while (*p == '/') p++;
        
        if (!node->fs->ops->finddir) return NULL;
        
        node = node->fs->ops->finddir(node, component);
        if (!node) return NULL;
    }
    
    return node;
}

/* Initialize the VFS */
void vfs_init(void) {
    memset(fd_table, 0, sizeof(fd_table));
    memset(mount_points, 0, sizeof(mount_points));
    print_str("[VFS] Initialized virtual filesystem\n");
}

/* Mount a filesystem */
int vfs_mount(const char* path, const char* device, const char* fs_type) {
    if (num_mounts >= MAX_MOUNTS) {
        print_str("[VFS] Error: Maximum number of mount points reached\n");
        return -1;
    }

    // TODO: Initialize filesystem driver based on fs_type
    struct FileSystem* fs = NULL;  // This should be allocated and initialized
    
    if (!fs) {
        print_str("[VFS] Error: Failed to initialize filesystem\n");
        return -1;
    }

    mount_points[num_mounts].path = strdup(path);
    mount_points[num_mounts].fs = fs;
    num_mounts++;

    print_str("[VFS] Mounted ");
    print_str((char*)fs_type);
    print_str(" at ");
    print_str((char*)path);
    print_str("\n");

    return 0;
}

/* Find a free file descriptor */
static int find_free_fd(void) {
    for (int i = 0; i < MAX_FD; i++) {
        if (fd_table[i].node == NULL) {
            return i;
        }
    }
    return -1;
}

/* Open a file */
int vfs_open(const char* path, uint32_t flags) {
    struct FSNode* node = vfs_lookup(path);
    if (!node) {
        print_str("[VFS] Error: File not found - ");
        print_str(path);
        print_str("\n");
        return -1;
    }

    int fd = find_free_fd();
    if (fd < 0) {
        print_str("[VFS] Error: No free file descriptors\n");
        return -1;
    }

    fd_table[fd].node = node;
    fd_table[fd].offset = 0;
    fd_table[fd].flags = flags;
    fd_table[fd].refcount = 1;

    if (node->fs && node->fs->ops && node->fs->ops->open) {
        return node->fs->ops->open(node, flags);
    }

    return fd;
}

/* Close a file */
int vfs_close(int fd) {
    if (fd < 0 || fd >= MAX_FD || !fd_table[fd].node) {
        return -1;
    }

    struct FSNode* node = fd_table[fd].node;
    if (node->fs && node->fs->ops && node->fs->ops->close) {
        node->fs->ops->close(node);
    }

    fd_table[fd].node = NULL;
    fd_table[fd].offset = 0;
    fd_table[fd].flags = 0;
    fd_table[fd].refcount = 0;

    return 0;
}

/* Read from a file */
int64_t vfs_read(int fd, void* buffer, uint64_t size) {
    if (fd < 0 || fd >= MAX_FD || !fd_table[fd].node) {
        return -1;
    }

    struct FSNode* node = fd_table[fd].node;
    if (!node->fs || !node->fs->ops || !node->fs->ops->read) {
        return -1;
    }

    int64_t bytes = node->fs->ops->read(node, fd_table[fd].offset, size, buffer);
    if (bytes > 0) {
        fd_table[fd].offset += bytes;
    }

    return bytes;
}

/* Write to a file */
int64_t vfs_write(int fd, const void* buffer, uint64_t size) {
    if (fd < 0 || fd >= MAX_FD || !fd_table[fd].node) {
        return -1;
    }

    struct FSNode* node = fd_table[fd].node;
    if (!node->fs || !node->fs->ops || !node->fs->ops->write) {
        return -1;
    }

    int64_t bytes = node->fs->ops->write(node, fd_table[fd].offset, size, buffer);
    if (bytes > 0) {
        fd_table[fd].offset += bytes;
    }

    return bytes;
}

/* Seek within a file */
int64_t vfs_seek(int fd, int64_t offset, int whence) {
    if (fd < 0 || fd >= MAX_FD || !fd_table[fd].node) {
        return -1;
    }

    struct FSNode* node = fd_table[fd].node;
    int64_t new_offset = fd_table[fd].offset;

    switch (whence) {
        case FS_SEEK_SET:
            new_offset = offset;
            break;
        case FS_SEEK_CUR:
            new_offset += offset;
            break;
        case FS_SEEK_END:
            new_offset = (int64_t)node->size + offset;
            break;
        default:
            return -1;
    }

    if (new_offset < 0 || (uint64_t)new_offset > node->size) {
        return -1;
    }

    fd_table[fd].offset = new_offset;
    return new_offset;
}

/* Create a directory */
int vfs_mkdir(const char* path, uint32_t mode) {
    // Find parent directory
    char* parent_path = vfs_normalize_path(path);
    char* last_slash = strrchr(parent_path, '/');
    if (!last_slash) {
        return -1;
    }
    *last_slash = '\0';
    
    struct FSNode* parent = vfs_lookup(parent_path);
    if (!parent) {
        return -1;
    }

    if (!parent->fs || !parent->fs->ops || !parent->fs->ops->mkdir) {
        return -1;
    }

    return parent->fs->ops->mkdir(parent, last_slash + 1, mode);
}

/* Check if path exists */
int vfs_exists(const char* path) {
    struct FSNode* node = vfs_lookup(path);
    return node != NULL;
}

/* Check if path is a directory */
int vfs_is_directory(const char* path) {
    struct FSNode* node = vfs_lookup(path);
    return node && (node->flags & FS_DIRECTORY);
}

/* Check if path is a regular file */
int vfs_is_file(const char* path) {
    struct FSNode* node = vfs_lookup(path);
    return node && (node->flags & FS_FILE);
} 