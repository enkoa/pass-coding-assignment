/**
 *  Project 3
 *  oufs_lib.c
 *
 *  Author: CS3113
 *
 *  Bao Le
 *  Project 3
 */

#include "oufs_lib.h"
#include "oufs_lib_support.h"
#include "virtual_disk.h"

// Yes ... a global variable
int debug = 1;

// Translate inode types to descriptive strings
const char *INODE_TYPE_NAME[] = {"UNUSED", "DIRECTORY", "FILE"};

/**
 Read the OUFS_PWD, OUFS_DISK, OUFS_PIPE_NAME_BASE environment
 variables copy their values into cwd, disk_name an pipe_name_base.  If these
 environment variables are not set, then reasonable defaults are
 given.

 @param cwd String buffer in which to place the OUFS current working directory.
 @param disk_name String buffer in which to place file name of the virtual disk.
 @param pipe_name_base String buffer in which to place the base name of the
            named pipes for communication to the server.

 PROVIDED
 */
void oufs_get_environment(char *cwd, char *disk_name,
			  char *pipe_name_base)
{
  // Current working directory for the OUFS
  char *str = getenv("OUFS_PWD");
  if(str == NULL) {
    // Provide default
    strcpy(cwd, "/");
  }else{
    // Exists
    strncpy(cwd, str, MAX_PATH_LENGTH-1);
  }

  // Virtual disk location
  str = getenv("OUFS_DISK");
  if(str == NULL) {
    // Default
    strcpy(disk_name, "vdisk1");
  }else{
    // Exists: copy
    strncpy(disk_name, str, MAX_PATH_LENGTH-1);
  }

  // Pipe name base
  str = getenv("OUFS_PIPE_NAME_BASE");
  if(str == NULL) {
    // Default
    strcpy(pipe_name_base, "pipe");
  }else{
    // Exists: copy
    strncpy(pipe_name_base, str, MAX_PATH_LENGTH-1);
  }

}

/**
 * Completely format the virtual disk (including creation of the space).
 *
 * NOTE: this function attaches to the virtual disk at the beginning and
 *  detaches after the format is complete.
 *
 * - Zero out all blocks on the disk.
 * - Initialize the master block: mark inode 0 as allocated and initialize
 *    the linked list of free blocks
 * - Initialize root directory inode 
 * - Initialize the root directory in block ROOT_DIRECTORY_BLOCK
 *
 * @return 0 if no errors
 *         -x if an error has occurred.
 *
 */

int oufs_format_disk(char  *virtual_disk_name, char *pipe_name_base)
{
  // Attach to the virtual disk
  if(virtual_disk_attach(virtual_disk_name, pipe_name_base) != 0) {
    return(-1);
  }

  BLOCK block;

  // Zero out the block
  memset(&block, 0, BLOCK_SIZE);
  for(int i = 0; i < N_BLOCKS; ++i) {
    if(virtual_disk_write_block(i, &block) < 0) {
      return(-2);
    }
  }

  //////////////////////////////
  // Master block
  block.next_block = UNALLOCATED_BLOCK;
  block.content.master.inode_allocated_flag[0] = 0x80;

  // TODO:  complete implementation AND DONE

  // set front and end of unallocated blocks
  block.content.master.unallocated_front = ROOT_DIRECTORY_BLOCK + 1;
  block.content.master.unallocated_end = N_BLOCKS - 1;
  
    // write master block to disk
  if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &block) < 0 ) {
    fprintf(stderr, "Error in writing master block to disk\n");
    return(-2);
  }
  
  //////////////////////////////
  // Root directory inode / block
  INODE inode;
  oufs_init_directory_structures(&inode, &block, ROOT_DIRECTORY_BLOCK,
				 ROOT_DIRECTORY_INODE, ROOT_DIRECTORY_INODE);

  // Write the results to the disk
  if(oufs_write_inode_by_reference(0, &inode) != 0) {
    return(-3);
  }

  // TODO: complete implementation AND DONE

  // write the directory block to the disk
  if (virtual_disk_write_block(ROOT_DIRECTORY_BLOCK, &block) < 0) {
    fprintf(stderr, "Error in writing root directory block to block 5\n");
    return(-6);
  }
  
  //////////////////////////////
  // All other blocks are free blocks

  // TODO: complete AND DONE

  // initialize block 6 to 126 and point them to the next reference
  BLOCK_REFERENCE j = ROOT_DIRECTORY_BLOCK + 1;
  while (j < N_BLOCKS - 1)  {
    BLOCK newBlock;
    memset(&newBlock, 0, BLOCK_SIZE);
    newBlock.next_block = j + 1;
    if (virtual_disk_write_block(j, &newBlock) < 0) {
      fprintf(stderr, "Fail to initialize free blocks\n");
      return(-4);
    }
    j++;
  }
  
  // initialize block 127 and have its next point to unallocated
  BLOCK lastFree;
  memset(&lastFree, 0, BLOCK_SIZE);
  lastFree.next_block = UNALLOCATED_BLOCK;
  if (virtual_disk_write_block(j, &lastFree) < 0) {
    fprintf(stderr ,"Fail to initialize last free block\n");
    return(-5);
  }
  
  
  // Done
  virtual_disk_detach();
 
  return(0);
}

/*
 * Compare two inodes for sorting, handling the
 *  cases where the inodes are not valid
 *
 * @param e1 Pointer to a directory entry
 * @param e2 Pointer to a directory entry
 * @return -1 if e1 comes before e2 (or if e1 is the only valid one)
 * @return  0 if equal (or if both are invalid)
 * @return  1 if e1 comes after e2 (or if e2 is the only valid one)
 *
 * Note: this function is useful for qsort()
 */
static int inode_compare_to(const void *d1, const void *d2)
{
  // Type casting from generic to DIRECTORY_ENTRY*
  DIRECTORY_ENTRY* e1 = (DIRECTORY_ENTRY*) d1;
  DIRECTORY_ENTRY* e2 = (DIRECTORY_ENTRY*) d2;

  // TODO: complete implementation AND DONE

  if (e2->inode_reference == UNALLOCATED_INODE) {
    return(-1);
  } else if (e1->inode_reference == UNALLOCATED_INODE) {
    return(1);
  } else {
    return(strcmp(e1->name, e2->name));
  }
  
}


/**
 * Print out the specified file (if it exists) or the contents of the 
 *   specified directory (if it exists)
 *
 * If a directory is listed, then the valid contents are printed in sorted order
 *   (as defined by strcmp()), one per line.  We know that a directory entry is
 *   valid if the inode_reference is not UNALLOCATED_INODE.
 *   Hint: qsort() will do to sort for you.  You just have to provide a compareTo()
 *   function (just like in Java!)
 *   Note: if an entry is a directory itself, then its name must be followed by "/"
 *
 * @param cwd Absolute path representing the current working directory
 * @param path Absolute or relative path to the file/directory
 * @return 0 if success
 *         -x if error
 *
 */

int oufs_list(char *cwd, char *path)
{
  INODE_REFERENCE parent;
  INODE_REFERENCE child;

  // Look up the inodes for the parent and child
  int ret = oufs_find_file(cwd, path, &parent, &child, NULL);

  // Did we find the specified file?
  if(ret == 0 && child != UNALLOCATED_INODE) {
    // Element found: read the inode
    INODE inode;
    if(oufs_read_inode_by_reference(child, &inode) != 0) {
      return(-1);
    }
    if(debug) 
      fprintf(stderr, "\tDEBUG: Child found (type=%s).\n",  INODE_TYPE_NAME[inode.type]);
    
    // TODO: complete implementation AND DONE
  
    BLOCK block;

    // read in reference block aka directory block
    if (virtual_disk_read_block(inode.content, &block) < 0) {
      fprintf(stderr, "Fail to read in block for oufs_list\n");
      return(-1);
    }

    // quick sort
    qsort(block.content.directory.entry, N_DIRECTORY_ENTRIES_PER_BLOCK, sizeof(DIRECTORY_ENTRY), inode_compare_to);

    // print out subdir/ file may change later for file
    int entryIndex = 0;
    INODE check;
    while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK) {
      
      if (block.content.directory.entry[entryIndex].inode_reference != UNALLOCATED_INODE) {
	
	// inode exist so get it to check whether or not it is a directory
	if (oufs_read_inode_by_reference(block.content.directory.entry[entryIndex].inode_reference, &check) < 0) {
	  fprintf(stderr, "Fail to get inode for checking in ls\n");
	  return(-1);
	}
	
	if (check.type == DIRECTORY_TYPE) {
	  printf("%s/\n", block.content.directory.entry[entryIndex].name);
	} else {
	  printf("%s\n", block.content.directory.entry[entryIndex].name);
	}
      }
      entryIndex++;
    }

  }else {
    // Did not find the specified file/directory
    fprintf(stderr, "Not found\n");
    if(debug)
      fprintf(stderr, "\tDEBUG: (%d)\n", ret);
  }
  // Done: return the status from the search
  return(ret);
}




///////////////////////////////////
/**
 * Make a new directory
 *
 * To be successful:
 *  - the parent must exist and be a directory
 *  - the parent must have space for the new directory
 *  - the child must not exist
 *
 * @param cwd Absolute path representing the current working directory
 * @param path Absolute or relative path to the file/directory
 * @return 0 if success
 *         -x if error
 *
 */
int oufs_mkdir(char *cwd, char *path)
{
  INODE_REFERENCE parent;
  INODE_REFERENCE child;

  // Name of a directory within another directory
  char local_name[MAX_PATH_LENGTH];
  int ret;

  // Attempt to find the specified directory
  if((ret = oufs_find_file(cwd, path, &parent, &child, local_name)) < -1) {
    if(debug)
      fprintf(stderr, "oufs_mkdir(): ret = %d\n", ret);
    return(-1);
  };

  // TODO: complete implementation AND DONE

  // dir name already exist if ret ==0
  if (ret == 0) {
    fprintf(stderr, "This directory name already existed\n");
    return(-2);
  }
  // If reach this point then it means ret == -1 with no fail
  // ret == -1 means child does not exist so name can be use

  // since name can be use, we will check to see if parent dir has space
  INODE parentInode;
  BLOCK block;

  // get parent inode and directory block
  if (oufs_read_inode_by_reference(parent, &parentInode) < 0) {
    fprintf(stderr, "Fail to read retrieve parent inode in mkdir\n");
    return(-1);
  }
  if(virtual_disk_read_block(parentInode.content, &block) < 0) {
    fprintf(stderr, "Fail to get parent directory block mkdir\n");
    return(-1);
  }

  // error if i size is full (size start at 1, so no need to -1 in entries)
  if (parentInode.size == N_DIRECTORY_ENTRIES_PER_BLOCK) {
    fprintf(stderr, "Directory is full cannot do mkdir\n");
    return(-4);
  }
  // if this point is reach then everything gucci, there is space

  // so now we allocate the new directory
  child = oufs_allocate_new_directory(parent);
  if (child == UNALLOCATED_INODE) {
    fprintf(stderr, "Fail to create new directory\n");
    return(-3);
  }

  // find the first empty entry, guarantee space bcause of check before
  int entryIndex = 0;
  while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK && block.content.directory.entry[entryIndex].inode_reference != UNALLOCATED_INODE) {
    entryIndex++;
  }
   
  // copy local name to dir entry name
  strcpy(block.content.directory.entry[entryIndex].name, local_name);
  
  // if name is larger, truncate
  if (strlen(local_name) >= FILE_NAME_SIZE-1) {
    block.content.directory.entry[entryIndex].name[FILE_NAME_SIZE - 1] = 0;
  }

  // point the entry inode reference to the child inode and increase size
  block.content.directory.entry[entryIndex].inode_reference = child;
  parentInode.size +=1;

  
  // write the inode and block back to disk
  if (oufs_write_inode_by_reference(parent, &parentInode) < 0) {
    fprintf(stderr, "Fail to write updated  parent inode to disk, mkdir\n");
    return(-6);
  }
  if (virtual_disk_write_block(parentInode.content, &block) < 0) {
    fprintf(stderr, "Fail to write updated parent dir block to disk, mkdir\n");
    return(-6);
  }
  

  return(0);
}

/**
 * Remove a directory
 *
 * To be successul:
 *  - The directory must exist and must be empty
 *  - The directory must not be . or ..
 *  - The directory must not be /
 *
 * @param cwd Absolute path representing the current working directory
 * @param path Abslute or relative path to the file/directory
 * @return 0 if success
 *         -x if error
 *
 */
int oufs_rmdir(char *cwd, char *path)
{
  INODE_REFERENCE parent;
  INODE_REFERENCE child;
  char local_name[MAX_PATH_LENGTH];

  // Try to find the inode of the child
  if(oufs_find_file(cwd, path, &parent, &child, local_name) < -1) {
    fprintf(stderr, "Subirectory/parent not found\n");
    return(-4);
  }

  // TODO: complet implementation AND DONE

  if (child == UNALLOCATED_INODE) {
    fprintf(stderr, "Name does not exist\n");
    return(-4);
  } 
  if (strcmp(local_name, ".") == 0 || strcmp(local_name, "..") == 0 || strcmp(local_name, "") == 0) {
    fprintf(stderr, "Error: cannot delete '.' or '..' or '/'\n");
    return(-1);
  }

  // if this point reached then
  // file name exist, path is legit,
  // and the name is not '.' or '..'

  // check whether or not this is a directory file
  // if is a diretory, check for emptiness

  INODE childInode;
  if (oufs_read_inode_by_reference(child, &childInode) < 0) {
    fprintf(stderr, "Fail to retrieve inode in rmdir\n");
    return(-1);
  }
  if (childInode.type != DIRECTORY_TYPE) {
    fprintf(stderr, "Specified name is not a directory\n");
    return(-2);
  }
  if (childInode.size > 2) {
    fprintf(stderr, "Directory not empty\n");
    return(-2);
  }

  // if this point is reah then
  // file exist with the right name and path
  // it is also a directory and it is empty
  // now proceed to deallocate directory

  // get master block
  BLOCK master;
  if (virtual_disk_read_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
    fprintf(stderr, "Fail to retrieve master block for rmdir\n");
    return(-1);
  }
  
  int ret = oufs_deallocate_block(&master, childInode.content);

  if (ret < 0) {
    fprintf(stderr, "Deallocation process for directory failed rmdir\n");
    return(-2);
  }

  // now the child inode block reference is gone
  
  // change the used bit in master block to unused using child inode ref
  // can also use 0x80 over 255
  int tableIndex = child / 8;
  int bitNum = child % 8;
  unsigned char daByte = 128;
  int byteVal = master.content.master.inode_allocated_flag[tableIndex];
    
  master.content.master.inode_allocated_flag[tableIndex] = byteVal & ~(daByte >> bitNum);

    
  // decrement parent inode size
  INODE parentInode;
  if (oufs_read_inode_by_reference(parent, &parentInode) < 0) {
    fprintf(stderr, "Fail to read in parent inode for rmdir\n");
    return(-1);
  }
  parentInode.size--;
  
  // now we have to use parent inode to get parent dir block and remove entry
  BLOCK parentBlock;
  if (virtual_disk_read_block(parentInode.content, &parentBlock) < 0) {
    fprintf(stderr, "Fail to retrieve parent dir block for modification rmdir\n");
    return(-1);
  }
  
  // find the entryIndex, it has to exist because of find file
  int entryIndex = 0;
  while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK) {
    // found matching name
    if (strcmp(parentBlock.content.directory.entry[entryIndex].name, local_name) == 0) {
      break;
    }
    entryIndex++;
  }

  // set name to blank and set inode to unallocated
  strcpy(parentBlock.content.directory.entry[entryIndex].name, "");
  parentBlock.content.directory.entry[entryIndex].inode_reference = UNALLOCATED_INODE;
  
  // write new parent inode/block, child inode, and master block back to memory

  if (oufs_write_inode_by_reference(parent, &parentInode) < 0) {
    fprintf(stderr, "Fail to write modified  parent inode to disk rmdir\n");
    return(-1);
  }
  if (oufs_write_inode_by_reference(child, &childInode) < 0) {
    fprintf(stderr, "Fail to write unallocated child inode to disk rmdir\n");
    return(-1);
  }
  if (virtual_disk_write_block(parentInode.content, &parentBlock) < 0) {
    fprintf(stderr, "Fail to write modified parent dir block to disk rmdir\n");
    return(-1);
  }
  if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
    fprintf(stderr, "Fail to write modified master block to disk rmdir\n");
    return(-1);
  }
  // Success
  return(0);
}
