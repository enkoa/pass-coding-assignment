/**
 *  Project 3
 *  oufs_lib_support.c
 *
 *  Author: CS3113
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "virtual_disk.h"
#include "oufs_lib_support.h"

extern int debug;

/**
 * Deallocate a single block.
 * - Modify the in-memory copy of the master block
 * - Add the specified block to THE END of the free block linked list
 * - Modify the disk copy of the deallocated block: next_block points to
 *     UNALLOCATED_BLOCK
 *   
 *
 * @param master_block Pointer to a loaded master block.  Changes to the MB will
 *           be made here, but not written to disk
 *
 * @param block_reference Reference to the block that is being deallocated
 *
 */
int oufs_deallocate_block(BLOCK *master_block, BLOCK_REFERENCE block_reference)
{
  BLOCK b;

  if(master_block->content.master.unallocated_front == UNALLOCATED_BLOCK) {
    // No blocks on the free list.  Both pointers point to this block now
    master_block->content.master.unallocated_front = master_block->content.master.unallocated_end =
      block_reference;

  }else{
    // TODO

    // get last free block
    BLOCK lastFree;
    if (virtual_disk_read_block(master_block->content.master.unallocated_end, &lastFree) < 0) {
      fprintf(stderr, "Fail to retrieve last block for modificaion in rmdir_sup\n");
      return(-1);
    }
    // point it to the deallocating block
    lastFree.next_block = block_reference;

    if (virtual_disk_write_block(master_block->content.master.unallocated_end, &lastFree) < 0) {
      fprintf(stderr, "Fail to write in the update last free bock (dealloc)\n");
      return(-1);
    }
    
    // point end to the deallocating block
    master_block->content.master.unallocated_end = block_reference;
    
  }

  // Update the new end block
  if(virtual_disk_read_block(block_reference, &b) != 0) {
    fprintf(stderr, "deallocate_block: error reading new end block\n");
    return(-1);
  }

  // Change the new end block to point to nowhere
  b.next_block = UNALLOCATED_BLOCK;

  // Write the block back
  if(virtual_disk_write_block(block_reference, &b) != 0) {
    fprintf(stderr, "deallocate_block: error writing new end block\n");
    return(-1);
  }

  return(0);
};


/**
 *  Initialize an inode and a directory block structure as a new directory.
 *  - Inode points to directory block (self_block_reference)
 *  - Inode size = 2 (for . and ..)
 *  - Direcory block: add entries . (self_inode_reference and .. (parent_inode_reference)
 *  -- Set all other entries to UNALLOCATED_BLOCK
 *
 * @param inode Pointer to inode structure to initialize
 * @param block Pointer to block structure to initialize as a directory
 * @param self_block_reference The block reference to the new directory block
 * @param self_inode_reference The inode reference to the new inode
 * @param parent_inode_reference The inode reference to the parent inode
 */
void oufs_init_directory_structures(INODE *inode, BLOCK *block,
				    BLOCK_REFERENCE self_block_reference,
				    INODE_REFERENCE self_inode_reference,
				    INODE_REFERENCE parent_inode_reference)
{
  // TODO AND DONE

  // initalize to directory type, 1 reference, content is self block ref, and size 2
  INODE_TYPE type = DIRECTORY_TYPE;
  oufs_set_inode(inode, type, 1, self_block_reference, 2);

  // initialize the block with "." and ".."  
  memset(block, 0, BLOCK_SIZE);
  strcpy(block->content.directory.entry[0].name, ".");
  block->content.directory.entry[0].inode_reference = self_inode_reference;
  strcpy(block->content.directory.entry[1].name, "..");
  block->content.directory.entry[1].inode_reference = parent_inode_reference;

  // set all other directory inode in the block to unallocated inode
  for (int i = 2; i < N_DIRECTORY_ENTRIES_PER_BLOCK; i ++) {
    block->content.directory.entry[i].inode_reference = UNALLOCATED_INODE;
  }

  // have the next block unallocated.
  block->next_block = UNALLOCATED_BLOCK;
}


/**
 *  Given an inode reference, read the inode from the virtual disk.
 *
 *  @param i Inode reference (index into the inode list)
 *  @param inode Pointer to an inode memory structure.  This structure will be
 *                filled in before return)
 *  @return 0 = successfully loaded the inode
 *         -1 = an error has occurred
 *
 */
int oufs_read_inode_by_reference(INODE_REFERENCE i, INODE *inode)
{
  if(debug)
    fprintf(stderr, "\tDEBUG: Fetching inode %d\n", i);

  // Find the address of the inode block and the inode within the block
  BLOCK_REFERENCE block = i / N_INODES_PER_BLOCK + 1;
  int element = (i % N_INODES_PER_BLOCK);

  // Load the block that contains the inode
  BLOCK b;
  if(virtual_disk_read_block(block, &b) == 0) {
    // Successfully loaded the block: copy just this inode
    *inode = b.content.inodes.inode[element];
    return(0);
  }
  // Error case
  return(-1);
}


/**
 * Write a single inode to the disk
 *
 * @param i Inode reference index
 * @param inode Pointer to an inode structure
 * @return 0 if success
 *         -x if error
 *
 */
int oufs_write_inode_by_reference(INODE_REFERENCE i, INODE *inode)
{
  if(debug)
    fprintf(stderr, "\tDEBUG: Writing inode %d\n", i);

  // TODO AND DONE
  
  BLOCK_REFERENCE block = i / N_INODES_PER_BLOCK + 1;
  int element = (i % N_INODES_PER_BLOCK);

  // fetch block
  BLOCK b;
  if (virtual_disk_read_block(block, &b) < 0) {
    fprintf(stderr, "Error fetching block to write inode to\n");
    return(-1);
  }
  
  // copy inode to element
  b.content.inodes.inode[element] = *inode;

  // write back to disk
  if (virtual_disk_write_block(block, &b) < 0 ) {
    fprintf(stderr, "Error writing inode to block\n");
    return(-1);
  }
  
  // Success
  return(0);
}

/**
 * Set all of the properties of an inode
 *
 * @param inode Pointer to the inode structure to be initialized
 * @param type Type of inode
 * @param n_references Number of references to this inode
 *          (when first created, will always be 1)
 * @param content Block reference to the block that contains the information within this inode
 * @param size Size of the inode (# of directory entries or size of file in bytes)
 *
 */

void oufs_set_inode(INODE *inode, INODE_TYPE type, int n_references,
		    BLOCK_REFERENCE content, int size)
{
  inode->type = type;
  inode->n_references = n_references;
  inode->content = content;
  inode->size = size;
}


/*
 * Given a valid directory inode, return the inode reference for the sub-item
 * that matches <element_name>
 *
 * @param inode Pointer to a loaded inode structure.  Must be a directory inode
 * @param element_name Name of the directory element to look up
 *
 * @return = INODE_REFERENCE for the sub-item if found; UNALLOCATED_INODE if not found
 */

int oufs_find_directory_element(INODE *inode, char *element_name)
{
  if(debug)
    fprintf(stderr,"\tDEBUG: oufs_find_directory_element: %s\n", element_name);

  // TODO AND DONE

  BLOCK block;

  // grab block from inode
  if (virtual_disk_read_block(inode->content, &block) < 0) {
    fprintf(stderr, "Fail to retrieve block from inode in oufs_find_directory_element\n");
    return(-1);
  }

  // read through all the entry in the block until find that entry or not
  int entryIndex = 0;
  while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK ) {
    if (block.content.directory.entry[entryIndex].inode_reference != UNALLOCATED_INODE) {
      if (strcmp(block.content.directory.entry[entryIndex].name, element_name) == 0) {
	return(block.content.directory.entry[entryIndex].inode_reference);
      }
    }
    entryIndex++;
  }
  
  return(UNALLOCATED_INODE);
}

/**
 *  Given a current working directory and either an absolute or relative path, find both the inode of the
 * file or directory and the inode of the parent directory.  If one or both are not found, then they are
 * set to UNALLOCATED_INODE.
 *
 *  This implementation handles a variety of strange cases, such as consecutive /'s and /'s at the end of
 * of the path (we have to maintain some extra state to make this work properly).
 *
 * @param cwd Absolute path for the current working directory
 * @param path Absolute or relative path of the file/directory to be found
 * @param parent Pointer to the found inode reference for the parent directory
 * @param child ointer to the found node reference for the file or directory specified by path
 * @param local_name String name of the file or directory without any path information
 *             (i.e., name relative to the parent)
 * @return 0 if no errors
 *         -1 if child not found
 *         -x if an error
 *
 */
int oufs_find_file(char *cwd, char * path, INODE_REFERENCE *parent, INODE_REFERENCE *child,
		   char *local_name)
{
  INODE_REFERENCE grandparent;
  char full_path[MAX_PATH_LENGTH];

  // Construct an absolute path the file/directory in question
  if(path[0] == '/') {
    strncpy(full_path, path, MAX_PATH_LENGTH-1);
  }else{
    if(strlen(cwd) > 1) {
      strncpy(full_path, cwd, MAX_PATH_LENGTH-1);
      strncat(full_path, "/", 2);
      strncat(full_path, path, MAX_PATH_LENGTH-1-strnlen(full_path, MAX_PATH_LENGTH));
    }else{
      strncpy(full_path, "/", 2);
      strncat(full_path, path, MAX_PATH_LENGTH-2);
    }
  }

  if(debug) {
    fprintf(stderr, "\tDEBUG: Full path: %s\n", full_path);
  };

  // Start scanning from the root directory
  // Root directory inode
  grandparent = *parent = *child = 0;
  if(debug)
    fprintf(stderr, "\tDEBUG: Start search: %d\n", *parent);

  // Parse the full path
  char *directory_name;
  directory_name = strtok(full_path, "/");
  while(directory_name != NULL) {
    if(strlen(directory_name) >= FILE_NAME_SIZE-1) 
      // Truncate the name
      directory_name[FILE_NAME_SIZE - 1] = 0;
    if(debug){
      fprintf(stderr, "\tDEBUG: Directory: %s\n", directory_name);
    }

    // TODO AND DONE

    INODE inode;
    BLOCK block;

    // retrieve current inode
    if (oufs_read_inode_by_reference(*child, &inode) < 0) {
      fprintf(stderr, "Fail to get inode: %d in oufs_find_file\n", *child);
      return(-2);
    }

    grandparent = *parent; // parent dir become grandparent
    *parent = *child; // current dir become parent
    *child = oufs_find_directory_element(&inode, directory_name); // method get next child

    // case for local name for mkdir and rmdir 
    if (local_name != NULL) {
      strcpy(local_name, directory_name);
    }

    //fprintf(stderr, "directory name before: %s\n", directory_name);
    directory_name = strtok(NULL, "/");
    //fprintf(stderr, "directory naem after: %s\n", directory_name);
    
    if (*child < 0) {
      fprintf(stderr, "-1 returned in oufs_find_directory_element\n");
      return(-2);
    } else if (*child == UNALLOCATED_INODE && directory_name == NULL) {
      // child does not exist, use for mkdir and rmdir
      return(-1);
    } else if (*child == UNALLOCATED_INODE && directory_name != NULL) {
      // one of the middle entry/parent does not exist
      fprintf(stderr, "One of the parent directory DNE (find_file)\n");
      return(-2);
    }

    
  };
  
  // this specified file exist if reach here

  // Item found.
  if(*child == UNALLOCATED_INODE) {
    // We went too far - roll back one step ***
    *child = *parent;
    *parent = grandparent;
  }
  if(debug) {
    fprintf(stderr, "\tDEBUG: Found: %d, %d\n", *parent, *child);
  }
  // Success!
  return(0);
} 


/**
 * Return the bit index for the first 0 bit in a byte (starting from 7th bit
 *   and scanning to the right)
 *
 * @param value: a byte
 * @return The bit number of the first 0 in value (starting from the 7th bit
 *         -1 if no zero bit is found
 */

int oufs_find_open_bit(unsigned char value)
{
  // TODO AND DONE

  int i = 7;
  for (i; i >=0; i--) {
    if ( ((value >> i) & 1) == 0 ) {
      return(7-i);
    }
  }
  
  // Not found
  return(-1);
}

/**
 *  Allocate a new directory (an inode and block to contain the directory).  This
 *  includes initialization of the new directory.
 *
 * @param parent_reference The inode of the parent directory
 * @return The inode reference of the new directory
 *         UNALLOCATED_INODE if we cannot allocate the directory
 */
int oufs_allocate_new_directory(INODE_REFERENCE parent_reference)
{
  BLOCK block;
  BLOCK block2;
  // Read the master block
  if(virtual_disk_read_block(MASTER_BLOCK_REFERENCE, &block) != 0) {
    // Read error
    return(UNALLOCATED_INODE);
  }


  
  // TODO AND DONE

  INODE_REFERENCE child;
  int tableIndex = 0;
  unsigned char byteVal = 0;
  for (tableIndex; tableIndex < N_INODES >> 3; tableIndex++) {
    byteVal = block.content.master.inode_allocated_flag[tableIndex];
    if (byteVal != 255) {
      // found index in table where there is non full byte
      break;
    }
  }
  // check to see if the exiting index from loop has free bit, if not then all bit are used
  if (byteVal == 255) {
    fprintf(stderr, "All bit are filled in inode table\n");
    return(UNALLOCATED_INODE);
  }

  // find open bit position in that byte (still need to add index*10 for inode number)
  child = oufs_find_open_bit(byteVal);

  unsigned char daByte = 128;
  
  // can also write 128 as 0x80
  block.content.master.inode_allocated_flag[tableIndex] = (unsigned char)(byteVal | (daByte >> child));

  // add index * 8  to get the actual Inode number (0-8)
  child += (tableIndex * 8 );

  // new inode and using unallocated front
  INODE inode, parentInode;
  BLOCK_REFERENCE blockRef = block.content.master.unallocated_front;

  // get inode from inode blocks
  if (oufs_read_inode_by_reference(child, &inode) < 0 ) {
    fprintf(stderr, "Fail to read in inode for mkdir support\n");
    return(UNALLOCATED_INODE);
  }
  // get first unallocated block
  if (virtual_disk_read_block(blockRef, &block2) < 0) {
    fprintf(stderr, "Fail to retrieve block %d from disk\n", blockRef);
    return(UNALLOCATED_INODE);
  }

  // point the the next unallcated front in master to the block after block2
  // or if front and end are same block, then that mean there is no more block
  if (block.content.master.unallocated_front != block.content.master.unallocated_end) {
    block.content.master.unallocated_front = block2.next_block;
  } else {
    block.content.master.unallocated_front = UNALLOCATED_BLOCK;
    block.content.master.unallocated_end = UNALLOCATED_BLOCK;
  }
  

  // initalize directory can be safe to edit to block 2 and inode
  oufs_init_directory_structures(&inode, &block2, blockRef, child, parent_reference);
  
  // now write master, block2, inode into memory

  if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &block) < 0) {
    fprintf(stderr, "Fail to write master block to disk in mkdir support\n");
    return(UNALLOCATED_BLOCK);
  }
  if (virtual_disk_write_block(blockRef, &block2) < 0) {
    fprintf(stderr, "Fail to write block2 to disk in mkdir support\n");
    return(UNALLOCATED_BLOCK);
  }
  if (oufs_write_inode_by_reference(child, &inode) < 0) {
    fprintf(stderr, "Fail to write inode to memory in mkdir support\n");
    return(UNALLOCATED_BLOCK);
  }

  return(child);
};




/************************************************************************/
// Project 4

/**
 *  Create a zero-length file within a specified diretory
 *
 *  @param parent Inode reference for the parent directory
 *  @param local_name Name of the file within the parent directory
 *  @return Inode reference index for the newly created file
 *          UNALLOCATED_INODE if an error
 *
 *  Errors include: virtual disk read/write errors, no available inodes,
 *    no available directory entrie
 */
INODE_REFERENCE oufs_create_file(INODE_REFERENCE parent, char *local_name)
{
  // Does the parent have a slot?
  INODE inode;

  // Read the parent inode
  if(oufs_read_inode_by_reference(parent, &inode) != 0) {
    return UNALLOCATED_INODE;
  }

  // Is the parent full?
  if(inode.size == N_DIRECTORY_ENTRIES_PER_BLOCK) {
    // Directory is full
    fprintf(stderr, "Parent directory is full.\n");
    return UNALLOCATED_INODE;
  }

  // TODO
  BLOCK master, parentBlock;
  //read master block
  if (virtual_disk_read_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
    // read error
    fprintf(stderr, "Fail to retrieve master block for create file\n");
    return(UNALLOCATED_INODE);
  }
  
  INODE_REFERENCE child;
  int tableIndex = 0;
  unsigned char byteVal = 0;

  for (tableIndex; tableIndex < N_INODES >> 3; tableIndex++) {
    byteVal = master.content.master.inode_allocated_flag[tableIndex];
    if (byteVal != 255) {
      // found a byte with open bit
      break;
    }
  }
  // check to to see if the loop didn't exit via break
  if (byteVal == 255) {
    fprintf(stderr, "All bit are filled in inode table\n");
    return(UNALLOCATED_INODE);
  }
  // find open bit in the byte if not 255
  child = oufs_find_open_bit(byteVal);

  unsigned char daByte = 128;

  master.content.master.inode_allocated_flag[tableIndex] = (byteVal | daByte >> child);

  child += (tableIndex *8);

  // read in the inode
  INODE childInode;
  if (oufs_read_inode_by_reference(child, &childInode) < 0) {
    fprintf(stderr, "Fail to read in inode for create file\n");
  }
  // set values
  childInode.type = FILE_TYPE;
  childInode.n_references = 1;
  childInode.content = UNALLOCATED_BLOCK;
  childInode.size = 0;

  // update parente directory
  if (virtual_disk_read_block(inode.content, &parentBlock) != 0) {
    fprintf(stderr, "Fail to get parent directory block in create file\n");
    return(UNALLOCATED_INODE);
  }
  // find open spot
  int entryIndex = 0;
  while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK && parentBlock.content.directory.entry[entryIndex].inode_reference != UNALLOCATED_INODE) {
    entryIndex++;
  }
  // update open spot
  parentBlock.content.directory.entry[entryIndex].inode_reference = child;
  strcpy(parentBlock.content.directory.entry[entryIndex].name, local_name);
  // increment inode size in parent inode
  inode.size++;


  // write back to memory masterblock, parentblock, childinode, parentinode
  if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &master) != 0) {
    fprintf(stderr, "Fail to write master block to memory create_file\n");
    return(UNALLOCATED_INODE);
  }
  if (virtual_disk_write_block(inode.content, &parentBlock) != 0) {
    fprintf(stderr, "Fail to write parent block to memory create_file\n");
    return(UNALLOCATED_INODE);
  }
  if (oufs_write_inode_by_reference(parent, &inode) < 0) {
    fprintf(stderr, "Fail to write parent inode to memory create_file\n");
    return(UNALLOCATED_INODE);
  }
  if (oufs_write_inode_by_reference(child, &childInode) < 0) {
    fprintf(stderr, "Fail to write child inode to memory create_file\n");
    return(UNALLOCATED_INODE);
  }
  // Success

  return(child);
}

/**
 * Deallocate all of the blocks that are being used by an inode
 *
 * - Modifies the inode to set content to UNALLOCATED_BLOCK
 * - Adds any content blocks to the end of the free block list
 *    (these are added in the same order as they are in the file)
 * - If the file is using no blocks, then return success without
 *    modifications.
 * - Note: the inode is not written back to the disk (we will let
 *    the calling function handle this)
 *
 * @param inode A pointer to an inode structure that is already in memory
 * @return 0 if success
 *         -x if error
 */

int oufs_deallocate_blocks(INODE *inode)
{
  BLOCK master_block;
  BLOCK block;

  // Nothing to do if the inode has no content
  if(inode->content == UNALLOCATED_BLOCK)
    return(0);

  // TODO

  // get master block;
  if(virtual_disk_read_block(MASTER_BLOCK_REFERENCE, &master_block) < 0) {
    fprintf(stderr, "Fail to readin master block\n");
    return(-1);
  }
  // get first block from file using inode
  if (virtual_disk_read_block(inode->content, &block) < 0) {
    fprintf(stderr, "Fail to read in block of the inode\n");
    return(-1);
  }
  // declare block reference for starting block and soon to be end block
  BLOCK_REFERENCE startBlock = inode->content;
  BLOCK_REFERENCE currentBlock = startBlock;

  while (startBlock != UNALLOCATED_BLOCK) {
    if (virtual_disk_read_block(currentBlock, &block) < 0) {
      fprintf(stderr, "Fail to read in block for deallocation loop\n");
      return(-1);
    }
    if(block.next_block == UNALLOCATED_BLOCK) {
      break;
    } else {
      currentBlock = block.next_block;
    }
  }
  // we found our start block and end block of this file!

  // 2 case: master has no more blocks or no
  if (master_block.content.master.unallocated_front == UNALLOCATED_BLOCK) {
    // no blocks on the free list. set start to front and current to end
    master_block.content.master.unallocated_front = startBlock;
    master_block.content.master.unallocated_end = currentBlock;
  } else {
    // get last free block
    BLOCK lastFree;
    if (virtual_disk_read_block(master_block.content.master.unallocated_end, &lastFree) < 0) {
      fprintf(stderr, "Fail to get last block in master for dellacoation\n");
      return(-1);
    }
    // point the unallocated end to the start block of the file
    lastFree.next_block = startBlock;
    // write the block back to memory
    if (virtual_disk_write_block(master_block.content.master.unallocated_end, &lastFree) < 0) {
      fprintf(stderr, "Fail to write last block to memory deallocation\n");
      return(-1);
    }
    // update unallocated end to the currentBlock, the last block in the file
    master_block.content.master.unallocated_end = currentBlock;
  }

  // write master block back to memory
  if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &master_block) < 0) {
    fprintf(stderr, "Fail to write master block back to memory deallcation\n");
    return(-1);
  }
  
  // update inode
  inode->content = UNALLOCATED_BLOCK;
  inode->size = 0;

  // Success
  return(0);
}

/**
 * Allocate a new data block
 * - If one is found, then the free block linked list is updated
 *
 * @param master_block A link to a buffer ALREADY containing the data from the master block.
 *    This buffer may be modified (but will not be written to the disk; we will let
 *    the calling function handle this).
 * @param new_block A link to a buffer into which the new block will be read.
 *
 * @return The index of the allocated data block.  If no blocks are available,
 *        then UNALLOCATED_BLOCK is returned
 *
 */
BLOCK_REFERENCE oufs_allocate_new_block(BLOCK *master_block, BLOCK *new_block)
{
  // Is there an available block?
  if(master_block->content.master.unallocated_front == UNALLOCATED_BLOCK) {
    // Did not find an available block
    if(debug)
      fprintf(stderr, "No blocks\n");
    return(UNALLOCATED_BLOCK);
  }

  // TODO
  BLOCK_REFERENCE ref = master_block->content.master.unallocated_front;
  // read in unallocated front to new block
  if(virtual_disk_read_block(ref, new_block) < 0) {
    fprintf(stderr, "Fail to read in new block from memory falloc\n");
    return(UNALLOCATED_BLOCK);
  }
  // update master unallocated front to new one
  if (master_block->content.master.unallocated_front == master_block->content.master.unallocated_end) {
    // unallocated front is the only free block left
    // point both front and end to unallocated block
    master_block->content.master.unallocated_front = UNALLOCATED_BLOCK;
    master_block->content.master.unallocated_end = UNALLOCATED_BLOCK;
  } else {
    // there is more block
    master_block->content.master.unallocated_front = new_block->next_block;
  }
  // set new block next block to unallocated
  new_block->next_block = UNALLOCATED_BLOCK;
  return(ref);
}


