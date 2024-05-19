#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "directory.h"
#include "slist.h"
#include "blocks.h"
#include "inode.h"
#include "bitmap.h"


// Maximum dir entries
// size of dirent = 64
int MAX_ENTR = 4096 / DIRENT_SIZE;

/*
 * Initializes the directory.
 */
void directory_init() {
    // Inode bitmap
	void* ibm =  get_inode_bitmap();	
	
	inode_t* root = get_inode(ROOT_INODE);

	// root->refs = 1;

    // Root is the inode root
	root->mode = 040755;
	root->size = 0;
	root->blocks[0] = alloc_block();
	root->blocks[1] = alloc_block();
    // Allocating blocks 1 and 2 to the root directory, since 0 is for bitmaps... 
    printf("Inode: ROOT_BLOCK0=%d, ROOT_BLOCK1=%d\n", root->blocks[0], root->blocks[1]);
	root->iblock = 0;

    // Sets the inode bitmap first spot to 1 for root
	bitmap_put(ibm, 0, 1);

    // Root to itself...
    directory_put(root, ".", ROOT_INODE);
}

int directory_lookup() {}

/*
 * Places an inode into the directory with the given inode number and name.
 * Takes in an inode, the string name, and the integer inode number.
 * Returns 0 upon success, or -1 on fail.
 */
int directory_put(inode_t *di, const char *name, int inum) {

    // entries in root directory
    dirent_t* entries = (dirent_t*) blocks_get_block(di->blocks[0]);

    // for all possible directory entries
    for (int i = 0; i < MAX_ENTR; i++) { //max entr is 64
		if(entries[i].inum == 0) { //pointer knows to increment +64
            entries[i].inum = inum;
			strcpy(entries[i].name, name);
            di->size += sizeof (dirent_t);
			return 0;
		}
	}
	return -1;
}