#include <stdlib.h>
#include <string.h>
#include "storage.h"
#include "inode.h"
#include "bitmap.h"
#include "blocks.h"
#include "directory.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

// Function to initialize the storage
void storage_init(const char *path) {
    // Initialize the disk image using blocks_init
    blocks_init(path); 

    // Now determine if disk needs to be initialized
    void* bbm = get_blocks_bitmap();

    if (bitmap_get(bbm, 0) == 0) {
        bitmap_put(bbm, 0, 1);
        directory_init();
    }
}

// Function to get file attributes
int storage_stat(const char *path, struct stat *st) {
    inode_t *node = get_inode(0);  // Assuming the root directory inode is at index 0
    if (node == NULL) {
        return -ENOENT;  // Root directory not found
    }

    st->st_mode = node->mode;
    st->st_size = node->size;
    // Other stat fields can be filled as needed

    return 0;
}

// Function to read data from a file
int storage_read(const char *path, char *buf, size_t size, off_t offset) {
    // Implementation based on your filesystem structure
    // You may need to modify it based on your actual file structure

    // Example: read the root directory data
    inode_t *node = get_inode(0);
    if (node == NULL) {
        return -ENOENT;  // Root directory not found
    }

    int block_num = inode_get_bnum(node, offset / BLOCK_SIZE);
    void *block_data = blocks_get_block(block_num);

    // Perform the actual read
    // You may need to handle reading from multiple blocks

    // Example: assuming a simplified scenario where the root directory data is stored in a single block
    memcpy(buf, block_data + offset % BLOCK_SIZE, size);

    return size;  // Return the number of bytes read
}

// Function to write data to a file
int storage_write(const char *path, const char *buf, size_t size, off_t offset) {
    // Similar to storage_read, you need to implement based on your filesystem structure

    // Example: write to the root directory data
    inode_t *node = get_inode(0);
    if (node == NULL) {
        return -ENOENT;  // Root directory not found
    }

    int block_num = inode_get_bnum(node, offset / BLOCK_SIZE);
    void *block_data = blocks_get_block(block_num);

    // Perform the actual write
    // You may need to handle writing to multiple blocks

    // Example: assuming a simplified scenario where the root directory data is stored in a single block
    memcpy(block_data + offset % BLOCK_SIZE, buf, size);

    // Update inode size if needed
    if (offset + size > node->size) {
        grow_inode(node, offset + size - node->size);
    }

    return size;  // Return the number of bytes written
}

// Function to truncate a file
int storage_truncate(const char *path, off_t size) {
    // You need to implement based on your filesystem structure

    // Example: truncate the root directory
    inode_t *node = get_inode(0);
    if (node == NULL) {
        return -ENOENT;  // Root directory not found
    }

    // Truncate the file by updating the inode size
    if (size < node->size) {
        shrink_inode(node, node->size - size);
    } else if (size > node->size) {
        grow_inode(node, size - node->size);
    }

    return 0;
}

// Function to create a file, directory, or special file
int storage_mknod(const char *path, int mode) {
    // You need to implement based on your filesystem structure

    // Example: create a new file (in this case, the root directory)
    if (alloc_inode() == -1) {
        return -ENOSPC;  // No space for new inode
    }

    // You may need to perform additional initialization if needed

    return 0;
}

// Function to delete a file or directory
int storage_unlink(const char *path) {
    // You need to implement based on your filesystem structure

    // Example: delete the root directory
    free_inode(0);  // Assuming the root directory inode is at index 0

    // You may need to free associated data blocks and perform additional cleanup

    return 0;
}

// Function to create a hard link
int storage_link(const char *from, const char *to) {
    // You need to implement based on your filesystem structure

    // Example: create a hard link
    // Assuming both 'from' and 'to' are valid paths, and the corresponding inodes are already allocated
    // You may need to update the inode reference count or block pointers

    return 0;
}

// Function to rename a file or directory
int storage_rename(const char *from, const char *to) {
    // You need to implement based on your filesystem structure

    // Example: rename the root directory
    // Assuming 'from' and 'to' are valid paths

    return 0;
}

// Function to set time information for a file or directory
int storage_set_time(const char *path, const struct timespec ts[2]) {
    // You need to implement based on your filesystem structure

    // Example: set time information for the root directory
    inode_t *node = get_inode(0);
    if (node == NULL) {
        return -ENOENT;  // Root directory not found
    }

    // Set time information as needed
    // You may need to update the inode fields

    return 0;
}

// Function to list files in a directory
slist_t *storage_list(const char *path) {
    // You need to implement based on your filesystem structure

    // Example: list files in the root directory
    return 0;
}
