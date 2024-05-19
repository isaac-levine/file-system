// Inode manipulation routines.
//
// Feel free to use as inspiration. Provided as-is.

// based on cs3650 starter code
#ifndef INODE_H
#define INODE_H
#include <stdint.h>

#include "blocks.h"

#define MAX_INODES 256  // Adjust the maximum number of inodes as needed

typedef struct inode {
  // int refs;  // reference count
  int mode;  // permission & type
  int size;  // bytes
  int blocks[2]; // block pointer (if max file size <= 4K)
  int iblock; // single indirect ptr
} inode_t;

void print_inode(inode_t *node); // done 
inode_t *get_inode(int inum); // 
int alloc_inode();
void free_inode();
int grow_inode(inode_t *node, int size);
int shrink_inode(inode_t *node, int size);
int inode_get_bnum(inode_t *node, int file_bnum);
void inodes_init();

#endif
