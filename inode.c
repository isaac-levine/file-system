#include <stdlib.h>
#include <stdint.h>
#include "inode.h"
#include "bitmap.h"
#include "blocks.h"

#define INODES_START 1

// for refernce - we have MAX_INODES set to 256 in the .h file

// Function to print an inode
void print_inode(inode_t *node) {
    printf("Inode: mode=%d, size=%d, block=%d\n", node->mode, node->size, node->blocks);
}

// Function to get an inode by its number
inode_t *get_inode(int inum) {
    if (inum >= 0 && inum < MAX_INODES) {
        // Calculate the offset of the inode in the block
        int offset = sizeof(inode_t) * inum;
        
        // Get a pointer to the beginning of the inode block
        void* inode_block = blocks_get_block(INODES_START); // Assuming inodes start from block 1

        // Return a pointer to the inode
        return (inode_t *)(inode_block + offset);
    }

    return NULL;  // Invalid inode number
}

// Finds the first free inode and marks it as used, then returns the index of it
int alloc_inode() {
    void *inode_bmap = get_inode_bitmap();
    for (int i = 0; i < MAX_INODES; ++i) {
        if (!bitmap_get(inode_bmap, i)) {
            // Inode is free
            bitmap_put(inode_bmap, i, 1);  // Mark inode as used

            // Initialize other inode fields for the inode
            inode_t* inode = get_inode(i);
            inode->mode = 0;  // Set the mode as needed
            inode->size = 0;
            inode->blocks[0] = alloc_block();  // Allocate a data block for the inode

            return i;
        }
    }
    return -1;  // No free inode available
}

// Function to free an inode
void free_inode(int inum) {
    void *inode_bmap = get_inode_bitmap();
    if (inum >= 0 && inum < MAX_INODES) { // check for valid inode index 
        bitmap_put(inode_bmap, inum, 0);  // Mark inode as used
        // reset other inode fields also?
    }
}

// Function to grow the size of an inode
int grow_inode(inode_t *node, int size) {
    // Increase the size of the inode and return the new size
    node->size += size;
    return node->size;
}

// Function to shrink the size of an inode
int shrink_inode(inode_t *node, int size) {
    // Decrease the size of the inode and return the new size
    node->size -= size;
    return node->size;
}

// Function to get the data block number for a given file block number in the inode
int inode_get_bnum(inode_t *node, int file_bnum) {
    if (file_bnum >= 0 && file_bnum < 12) {
        // Direct block pointers
        return node->blocks[0] + file_bnum;
    } else {
        // Indirect blocks are not supported in this simplified example
        return -1;
    }
}

// we added this -- not necessary
// // Function to initialize the inodes // not necessary????
// void inodes_init() {
//     printf("Calling inodes_init\n");
//     // Assuming inode blocks start from block 1
//     void* inode_block = blocks_get_block(INODES_START);

//     for (int i = 0; i < MAX_INODES; ++i) {
//         // Get a pointer to the current inode
//         inode_t* inode = (inode_t*)(inode_block + sizeof(inode_t) * i);

//         // Initialize the inode fields
//         inode->mode = 0;  // Set the mode as needed
//         inode->size = 0;
//         inode->block = -1;  // Initialize to an invalid block number
//     }
// }