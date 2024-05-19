// based on cs3650 starter code

#include <assert.h>
#include <bsd/string.h>
// #include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "blocks.h"
#include "bitmap.h" 
#include "inode.h" // all our inode stuff
#include "storage.h" 
#include "directory.h"

#define FUSE_USE_VERSION 26
#include <fuse.h>

#define MAX_INODES 256  // set to same as the # of blocks for now 
#define BLOCK_COUNT 256
#define SIZE 256

#define MAX_FILE_NAME_LENGTH 10 // verify this with prof/TA

// implementation for: man 2 access
// Checks if a file exists.
int nufs_access(const char *path, int mask) {
  int rv = 0;


  // // need to implement tree search TODO
  // if (tree_search(path) == -1) {
  //   rv = -ENOENT;
  // }

  printf("access(%s, %04o) -> %d\n", path, mask, rv);
  return rv;
}

// Gets an object's attributes (type, permissions, size, etc).
// Implementation for: man 2 stat
// This is a crucial function.
int nufs_getattr(const char *path, struct stat *st) {
  int rv = 0;

  if (strcmp(path, "/") == 0) {

    inode_t* root = get_inode(ROOT_INODE);

    st->st_size = root->size;
    st->st_mode = root->mode;
    st->st_uid = getuid();
    printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
      st->st_size);
    return rv;
  }


  return -ENOENT;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
  // ------ COMMENTED OUT STARTER CODE ------- 

  struct stat st;
  int rv;

  rv = nufs_getattr("/", &st);
  assert(rv == 0);

  filler(buf, ".", &st, 0);

  // rv = nufs_getattr("/hello.txt", &st);
  // assert(rv == 0);
  // filler(buf, "hello.txt", &st, 0);

  printf("readdir(%s) -> %d\n", path, rv);
  return 0;

  // end starter code

  // ACTUAL WORK BELOW, COMMENTED OUT FOR TESTING SAKE

  // // Check if the path is the root directory (is this necessary?)
  // if (strcmp(path, "/") != 0) {
  //   return -ENOENT; 
  // }

  // // Iterate over inodes to get directory entries
  // for (int inum = 0; inum < MAX_INODES; ++inum) {
  //   inode_t *node = get_inode(inum);

  //   // Check if the inode is in use
  //   if (node->mode != 0) {
  //     // Add the directory entry using filler - provided by fuse to add entries to the buffer 'buf'
  //     if (filler(buf, get_filename(inum), NULL, 0) != 0) {
  //       // The buffer is full, stop adding entries
  //       return -ENOMEM;
  //     }
  //   }
  // }
  // return 0; // success
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
// Note, for this assignment, you can alternatively implement the create
// function.
int nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
  printf("Calling nufs_mknod\n");
  // Check if the file name is up to 10 characters
  if (strlen(path) > MAX_FILE_NAME_LENGTH) {
    printf("File name too long\n");
    return -ENAMETOOLONG;  // File name too long
  }

  // // init the blocks
  // blocks_init(path); // doing this in storage_init instead
  // 

  // Find a free inode and mark it as occupied, inum is the index of it
  int inum = alloc_inode();
  // if there is no free inode, return error 
  if (inum == -1) { 
    printf("No free inode (returned from nufs_mknod)\n");
    return -ENOSPC;  
  }

  // Get the inode structure for the allocated inode
  inode_t *new_inode = get_inode(inum);

  // Set the mode of the new inode (e.g., regular file or directory)
  new_inode->mode = mode;

  // Set the size of the new inode (initially 0 for an empty file or directory)
  new_inode->size = 0;

  // Allocate a data block for the new inode (this could be more sophisticated)
  int data_block = alloc_block();
  // if there are no free data blocks, return error 
  if (data_block == -1) {
    printf("No free data blocks (returned from nufs_mknod)\n");
    free_inode(inum); 
    return -ENOSPC;  
  }

  // Set the block pointer of the new inode to the allocated data block
  new_inode->blocks[0] = data_block;

  printf("nufs_mknod succeeded\n");
  return 0; // success 

}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int nufs_mkdir(const char *path, mode_t mode) {
  int rv = nufs_mknod(path, mode | 040000, 0);
  printf("mkdir(%s) -> %d\n", path, rv);
  return rv;
}

int nufs_unlink(const char *path) {
  int rv = -1;
  printf("unlink(%s) -> %d\n", path, rv);
  return rv;
}

int nufs_link(const char *from, const char *to) {
  int rv = -1;
  printf("link(%s => %s) -> %d\n", from, to, rv);
  return rv;
}

int nufs_rmdir(const char *path) {
  int rv = -1;
  printf("rmdir(%s) -> %d\n", path, rv);
  return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int nufs_rename(const char *from, const char *to) {
  int rv = -1;
  printf("rename(%s => %s) -> %d\n", from, to, rv);
  return rv;
}

int nufs_chmod(const char *path, mode_t mode) {
  int rv = -1;
  printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
  return rv;
}

int nufs_truncate(const char *path, off_t size) {
  int rv = -1;
  printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
  return rv;
}

// This is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
// You can just check whether the file is accessible.
int nufs_open(const char *path, struct fuse_file_info *fi) {
  int rv = 0;
  printf("open(%s) -> %d\n", path, rv);
  return rv;
}

// Actually read data
int nufs_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
  int rv = 6;
  strcpy(buf, "hello\n");
  printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
  return rv;
}

// Actually write data
int nufs_write(const char *path, const char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi) {
  int rv = -1;
  printf("write(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
  return rv;
}

// Update the timestamps on a file or directory.
int nufs_utimens(const char *path, const struct timespec ts[2]) {
  int rv = -1;
  printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n", path, ts[0].tv_sec,
         ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
  return rv;
}

// Extended operations
int nufs_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi,
               unsigned int flags, void *data) {
  int rv = -1;
  printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
  return rv;
}

void nufs_init_ops(struct fuse_operations *ops) {
  memset(ops, 0, sizeof(struct fuse_operations));
  ops->access = nufs_access;
  ops->getattr = nufs_getattr;
  ops->readdir = nufs_readdir;
  ops->mknod = nufs_mknod;
  // ops->create   = nufs_create; // alternative to mknod
  ops->mkdir = nufs_mkdir;
  ops->link = nufs_link;
  ops->unlink = nufs_unlink;
  ops->rmdir = nufs_rmdir;
  ops->rename = nufs_rename;
  ops->chmod = nufs_chmod;
  ops->truncate = nufs_truncate;
  ops->open = nufs_open;
  ops->read = nufs_read;
  ops->write = nufs_write;
  ops->utimens = nufs_utimens;
  ops->ioctl = nufs_ioctl;
};

struct fuse_operations nufs_ops;

int main(int argc, char *argv[]) {
  assert(argc > 2 && argc < 6);
  
  // init blocks image for given disk
  storage_init(argv[--argc]); 

  nufs_init_ops(&nufs_ops);
  return fuse_main(argc, argv, &nufs_ops, NULL);
}
