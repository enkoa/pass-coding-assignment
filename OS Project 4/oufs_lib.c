
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

  // PROJECT 4 ADDTION
  childInode.type = UNUSED_TYPE;
  childInode.content = UNALLOCATED_BLOCK;
  childInode.size = 0;
  childInode.n_references = 0;
  
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






int setOUFILE (OUFILE *fp, char mode, INODE_REFERENCE ref) {
  INODE inode;
  if (oufs_read_inode_by_reference(ref, &inode) < 0) {
    fprintf(stderr, "Fail to get inode for setOUFILE\n");
    return(-1);
  }
  // have inode now set base on mode
  if (mode == 'a') {
    fp->inode_reference = ref;
    fp->mode = mode;
    fp->offset = inode.size;

    return(0);
  } else if (mode == 'r') {
    fp->inode_reference = ref;
    fp->mode = mode;
    fp->offset = 0;
    
    return(0);
  } else if (mode == 'w') {
    fp->inode_reference = ref;
    fp->mode = mode;
    fp->offset = inode.size;
    
    return(0);
  } else {
    fprintf(stderr, "Mode wrong setOUFILE\n");
    return(-1);
  }
}



/*********************************************************************/
// Project 4
/**
 * Open a file
 * - mode = "r": the file must exist; offset is set to 0
 * - mode = "w": the file may or may not exist;
 *                 - if it does not exist, it is created 
 *                 - if it does exist, then the file is truncated
 *                       (size=0 and data blocks deallocated);
 *                 offset = 0 and size = 0
 * - mode = "a": the file may or may not exist
 *                 - if it does not exist, it is created 
 *                 offset = size
 *
 * @param cwd Absolute path for the current working directory
 * @param path Relative or absolute path for the file in question
 * @param mode String: one of "r", "w" or "a"
 *                 (note: only the first character matters here)
 * @return Pointer to a new OUFILE structure if success
 *         NULL if error
 */
OUFILE* oufs_fopen(char *cwd, char *path, char *mode)
{
  INODE_REFERENCE parent;
  INODE_REFERENCE child;
  char local_name[MAX_PATH_LENGTH];
  INODE inode;
  int ret;

  // Check for valid mode
  if(mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a') {
    fprintf(stderr, "fopen(): bad mode.\n");
    return(NULL);
  };

  // Try to find the inode of the child
  if((ret = oufs_find_file(cwd, path, &parent, &child, local_name)) < -1) {
    if(debug)
      fprintf(stderr, "oufs_fopen(%d)\n", ret);
    return(NULL);
  }
  
  if(parent == UNALLOCATED_INODE) {
    fprintf(stderr, "Parent directory not found.\n");
    return(NULL);
  }

  // TODO
  OUFILE *fp;
  INODE childInode;
  
  if (ret == 0) {
    // child exist get inode
    if (oufs_read_inode_by_reference(child, &inode) < 0) {
      fprintf(stderr, "Error getting inode for oufs_open\n");
      return(NULL);
    }
    // use inode to check if it is a directory
    if (inode.type == DIRECTORY_TYPE) {
      fprintf(stderr, "File name is a directory\n");
      return(NULL);
    }
    // if reached here, then child exist and it is not a directory
  } else if (ret == -1) {
    // child does not exist, so we make one and get inode reference
    // we dont make one but it is an error if the mode is read
    if (mode[0] == 'r') {
      fprintf(stderr, "Child/file name does not exist\n");
      return(NULL);
    }
    child = oufs_create_file(parent, local_name);
  } else {
    fprintf(stderr, "Should never reach here fopen\n");
    return(NULL);
  }

  // now child inode reference should referencing child
  fp = malloc(sizeof(OUFILE));
  
  if (mode[0] == 'a') {
    // since it is append we set OUFILE stuff with a
    if (setOUFILE(fp, 'a', child) < 0) {
      fprintf(stderr, "Error in setting attribute for file open_file\n");
      free(fp);
      return(NULL);
    }
  } else if (mode[0] == 'r') {
    if (setOUFILE(fp, 'r', child) < 0) {
      fprintf(stderr, "Error in setting attribute for file open 'r'\n");
      free(fp);
      return(NULL);
    }
  } else if (mode[0] == 'w') {
    // child inode reference exist now so get the inode
    if (oufs_read_inode_by_reference(child, &childInode) < 0) {
      fprintf(stderr, "Fail to get inode for childe open_file\n");
      return(NULL);
    }
    // deallocate inode, if inode is new then content should be unallocated
    if (oufs_deallocate_blocks(&childInode) < 0) {
      fprintf(stderr, "Fail to deallocate file block\n");
      return(NULL);
    }
    // set the FILE
    if (setOUFILE(fp, 'w', child) < 0) {
      fprintf(stderr, "Fail to make OUFILe open_file for 'w'\n");
      free(fp);
      return(NULL);
    }

    // write the updated child inode back to memory that we used
    if (oufs_write_inode_by_reference(child, &childInode) < 0) {
      fprintf(stderr, "Fail to write child inode back to disk open_file\n");
      free(fp);
      return(NULL);
    }
  } else {
    fprintf(stderr, "Should never reach here\n");
    free(fp);
    return(NULL);
  }

  //fprintf(stderr, "OUFILE inode ref: %d\n", fp->inode_reference);
  //fprintf(stderr, "OUFILE mode: %c\n", fp->mode);
  //fprintf(stderr, "OUFILE offset: %d\n\n", fp->offset);
  
  return(fp);
};

/**
 *  Close a file
 *   Deallocates the OUFILE structure
 *
 * @param fp Pointer to the OUFILE structure
 */
     
void oufs_fclose(OUFILE *fp) {
  fp->inode_reference = UNALLOCATED_INODE;
  free(fp);
}



/*
 * Write bytes to an open file.
 * - Allocate new data blocks, as necessary
 * - Can allocate up to MAX_BLOCKS_IN_FILE, at which point, no more bytes may be written
 * - file offset will always match file size; both will be updated as bytes are written
 *
 * @param fp OUFILE pointer (must be opened for w or a)
 * @param buf Character buffer of bytes to write
 * @param len Number of bytes to write
 * @return The number of written bytes
 *          0 if file is full and no more bytes can be written
 *         -x if an error
 * 
 */
int oufs_fwrite(OUFILE *fp, unsigned char * buf, int len)
{
  if(fp->mode == 'r') {
    fprintf(stderr, "Can't write to read-only file");
    return(0);
  }
  if(debug)
    fprintf(stderr, "-------\noufs_fwrite(%d)\n", len);
    
  INODE inode;
  BLOCK block;
  if(oufs_read_inode_by_reference(fp->inode_reference, &inode) != 0) {
    return(-1);
  }

  // Compute the index for the last block in the file + the first free byte within the block
  
  int current_blocks = (fp->offset + DATA_BLOCK_SIZE - 1) / DATA_BLOCK_SIZE;
  int used_bytes_in_last_block = fp->offset % DATA_BLOCK_SIZE;
  int free_bytes_in_last_block = DATA_BLOCK_SIZE - used_bytes_in_last_block;
  int len_written = 0;

  // TODO
  BLOCK master;
  BLOCK_REFERENCE currentBlock, testBlock;
  int freeByte, usedByte;
  
  // check if file is full
  if (inode.size >= (DATA_BLOCK_SIZE*MAX_BLOCKS_IN_FILE)) {
    fprintf(stderr, "Maximum number of block reached\n");
    return(0);
  }
  // get master block, good chance we'll need it
  if (virtual_disk_read_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
    fprintf(stderr, "Fail to get master block for fwrite\n");
    return(-1);
  }

  
  // 2 case: file is has no block or it does
  // get new block or find the last block
  if (inode.content == UNALLOCATED_BLOCK) {
    fprintf(stderr, "0 block file\n");
    // file has no block, allocate one and get its reference
    currentBlock = oufs_allocate_new_block(&master, &block);
    if (currentBlock == UNALLOCATED_BLOCK) {
      fprintf(stderr, "Cannot allocate any block, for there is no more\n");
      return(-1);
    }
    inode.content = currentBlock;
    freeByte = DATA_BLOCK_SIZE;
    usedByte = 0;
  } else {
    fprintf(stderr, "has block file\n");
    // loop to find the last file
    currentBlock = inode.content;
    while(1) {
      if (virtual_disk_read_block(currentBlock, &block) < 0) {
	fprintf(stderr, "Fail to get last block in file for fwrite\n");
	return(-1);
      }
      if (block.next_block == UNALLOCATED_BLOCK) {
	// this is the last block! exit!
	break;
      } else {
	// move to current block to next ref
	currentBlock = block.next_block;
      }
    }
    if (free_bytes_in_last_block == DATA_BLOCK_SIZE) {
      freeByte = 0;
      usedByte = DATA_BLOCK_SIZE;
    } else {
      freeByte = free_bytes_in_last_block;
      usedByte = used_bytes_in_last_block;
    }
  }

  // now we have a working block either with data or not

  int bufHead = len_written;
  while (len_written < len) {
    //fprintf(stderr, "SIZE: %d\n", inode.size);
    // check if data already full
    if (inode.size >= (DATA_BLOCK_SIZE*MAX_BLOCKS_IN_FILE)) {
      fprintf(stderr, "Used all blocks available for single file\n");
      //len_written = 0;
      break;
    }
    
    // 3 cases: last block full, last block > len, last block < len
    if (freeByte == 0) {
      fprintf(stderr, "Case 0\n");
      // allocate new block
      block.next_block = master.content.master.unallocated_front;
      if (virtual_disk_write_block(currentBlock, &block) < 0) {
	fprintf(stderr, "Fail to write block back to memeory fwrite loop2\n");
	return(-1);
      }
      testBlock = oufs_allocate_new_block(&master, &block);
      if (testBlock == UNALLOCATED_BLOCK) {
	fprintf(stderr, "No more block, we used all from master\n");
	len_written = 0;
	break;
      }
      currentBlock = testBlock;
      // now we have more freebyte!
      freeByte = DATA_BLOCK_SIZE;
      usedByte = 0;
    } else if (len - len_written > DATA_BLOCK_SIZE) {
      fprintf(stderr, "Case 1\n");
      // write to a whole block then reloop to get next block
      memcpy(&block.content.data.data[usedByte], &buf[bufHead], freeByte);

      fp->offset += freeByte;
      len_written += freeByte;
      inode.size += freeByte;
      bufHead += freeByte;
      usedByte = DATA_BLOCK_SIZE;
      freeByte = 0;
    } else if (len - len_written <= DATA_BLOCK_SIZE) {
      // but does it have enough space for the remaining?
      if ((len-len_written) > freeByte) {
	fprintf(stderr, "Case 2 part 1\n");
	// it has more byte than free space, write what we can in
	memcpy(&block.content.data.data[usedByte], &buf[bufHead], freeByte);
	// record offset
	fp->offset += freeByte;
	inode.size += freeByte;
	bufHead += freeByte;
	len_written += freeByte;
	usedByte = DATA_BLOCK_SIZE;
	freeByte = 0;
      } else {
	fprintf(stderr, "Case 2 part 2\n");
	// it has less so we just add it in
	memcpy(&block.content.data.data[usedByte], &buf[bufHead], (len-len_written));
	fp->offset += (len-len_written);
	inode.size += (len-len_written);
	bufHead += (len-len_written);
	usedByte += (len-len_written);
	freeByte -= (len-len_written);
	len_written += (len-len_written);
      }
    } else {
      fprintf(stderr, "Should never reach here\n");
      return(-100);
    }
  }

  
  //fprintf(stderr, "Current data in block: %s\n", block.content.data.data);
  fprintf(stderr, "Offset/size: %d/%d\n", fp->offset, inode.size);
    
  // write back to disk
  if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
    fprintf(stderr, "Fail to write master block back to disk\n");
    return(-1);
  }
  if(virtual_disk_write_block(currentBlock, &block) < 0) {
    fprintf(stderr, "Fail to write in the lastblock for file fwrite\n");
    return(-1);
  }
  if (oufs_write_inode_by_reference(fp->inode_reference ,&inode) < 0) {
    fprintf(stderr, "Fail to write file inode back to memory fwrite\n");
    return(-1);
  }
  // Done
  return(len_written);
}


/*
 * Read a sequence of bytes from an open file.
 * - offset is the current position within the file, and will never be larger than size
 * - offset will be updated with each read operation
 *
 * @param fp OUFILE pointer (must be opened for r)
 * @param buf Character buffer to place the bytes into
 * @param len Number of bytes to read at max
 * @return The number of bytes read
 *         0 if offset is at size
 *         -x if an error
 * 
 */

int oufs_fread(OUFILE *fp, unsigned char * buf, int len)
{
  // Check open mode
  if(fp->mode != 'r') {
    fprintf(stderr, "Can't read from a write-only file");
    return(0);
  }
  if(debug)
    fprintf(stderr, "\n-------\noufs_fread(%d)\n", len);
    
  INODE inode;
  BLOCK block;
  if(oufs_read_inode_by_reference(fp->inode_reference, &inode) != 0) {
    return(-1);
  }
      
  // Compute the current block and offset within the block
  int current_block = fp->offset / DATA_BLOCK_SIZE;
  int byte_offset_in_block = fp->offset % DATA_BLOCK_SIZE;
  int len_read = 0;
  int end_of_file = inode.size;
  len = MIN(len, end_of_file - fp->offset);
  int len_left = len;

  // TODO
  BLOCK_REFERENCE ref = inode.content;
  BLOCK_REFERENCE curRef;
  // if file is empty
  fprintf(stderr, "START OFFSET: %d\n", fp->offset);
  if (fp->offset >= end_of_file) {
    return(0);
  }
  // get first block
  if (virtual_disk_read_block(ref, &block) < 0) {
    fprintf(stderr, "Fail to get inode.content\n");
    return(-1);
  }
  // get current block
  for (int i = 0; i < current_block; i++) {
    ref = block.next_block;
    if (virtual_disk_read_block(ref, &block) < 0) {
      fprintf(stderr, "Fail to get block for fwrite\n");
      return(0);
    }
  }
  fprintf(stderr, "START BLOCK: %d\n", ref);
    
  int head = byte_offset_in_block;
  // remain bytes in the current block
  int remain = DATA_BLOCK_SIZE - byte_offset_in_block;
  while (len_read < len_left){
    head = fp->offset % DATA_BLOCK_SIZE;
    // 3 case, remaining read can fit in 1 block or not
    if (remain == 0) {
      fprintf(stderr, "Case 0: read all from current block, getting next one\n");
      // remain == 0 mean we read all the byte from this block already,
      // get next block
      ref = block.next_block;
      if (ref == UNALLOCATED_BLOCK) {
	fprintf(stderr, "should never reach here in theory, but");
	fprintf(stderr, " next block to read from is unallocated\n");
	return(-1);
      }
      // read in block
      if (virtual_disk_read_block(ref, &block) < 0) {
	fprintf(stderr, "Fail to read in next block fread loop\n");
	return(-1);
      }
      fprintf(stderr, "CURRENT BLOCK: %d\n", ref);
      // we have more byte now so update remain to max
      remain = DATA_BLOCK_SIZE;
    } else if ((len_left-len_read) > remain) {
      fprintf(stderr, "Case 1: remaining is more than a block\n");
      // write what we can into the current block, with offset in mind
      memcpy(&buf[len_read], &block.content.data.data[head], remain);
      // update offset, len read and use all remain
      fp->offset += remain;
      len_read += remain;
      remain = 0;
      fprintf(stderr, "OFFSET: %d\n", fp->offset);
    } else if ((len_left-len_read) < remain) {
      fprintf(stderr, "Case 2: remaining is less than a block\n");
      // write in what we have left
      memcpy(&buf[len_read], &block.content.data.data[head], (len_left-len_read));
      // update offset, len read and reduce remain;
      fp->offset += (len_left-len_read);
      len_read += (len_left-len_read);
      remain -= (len_left-len_read);
      fprintf(stderr, "OFFSET: %d\n", fp->offset);
    } else {
      fprintf(stderr, "Should never reach here fread\n");
      return(-1);
    }
  }

  //fprintf(stderr, "len_read: %d\n",len_read);
  // Done
  return(len_read);
}


/**
 * Remove a file
 *
 * Full implementation:
 * - Remove the directory entry
 * - Decrement inode.n_references
 * - If n_references == 0, then deallocate the contents and the inode
 *
 * @param cwd Absolute path for the current working directory
 * @param path Absolute or relative path of the file to be removed
 * @return 0 if success
 *         -x if error
 *
 */

int oufs_remove(char *cwd, char *path)
{
  INODE_REFERENCE parent;
  INODE_REFERENCE child;
  char local_name[MAX_PATH_LENGTH];
  INODE inode;
  INODE inode_parent;
  BLOCK block;

  // Try to find the inode of the child
  if(oufs_find_file(cwd, path, &parent, &child, local_name) < -1) {
    return(-3);
  };
  
  if(child == UNALLOCATED_INODE) {
    fprintf(stderr, "File not found\n");
    return(-1);
  }
  // Get the inode
  if(oufs_read_inode_by_reference(child, &inode) != 0) {
    return(-4);
  }

  // Is it a file?
  if(inode.type != FILE_TYPE) {
    // Not a file
    fprintf(stderr, "Not a file\n");
    return(-2);
  }

  // TODO
  BLOCK master;
  // read in parent inode
  if (oufs_read_inode_by_reference(parent, &inode_parent) < 0) {
    fprintf(stderr, "Fail to get parent inode rmfile\n");
    return(-1);
  }
  // get parent block
  if (virtual_disk_read_block(inode_parent.content, &block) < 0) {
    fprintf(stderr, "Fail to get parent block\n");
    return(-1);
  }
  // find file name and delete;
  int entryIndex = 0; 
  while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK) {
    if (strcmp(block.content.directory.entry[entryIndex].name, local_name) == 0) {
      break;
    }
    entryIndex++;
  }
  strcpy(block.content.directory.entry[entryIndex].name, "");
  block.content.directory.entry[entryIndex].inode_reference = UNALLOCATED_INODE;
  // decrement parent
  inode_parent.size--;

  inode.n_references--;
  // no more link to this file. so we delete
  if (inode.n_references == 0) {
    if (oufs_deallocate_blocks(&inode) < 0) {
      fprintf(stderr, "Fail to deallocate blocks rmfile\n");
      return(-1);
    }
    inode.type = UNUSED_TYPE;
    // get master block and update
    if (virtual_disk_read_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
      fprintf(stderr, "Fail to get master block for rmfile\n");
      return(-1);
    }

    // information we need to update master
    int tableIndex = child / 8;
    int bitNum = child % 8;
    unsigned char daByte = 128;
    int byteVal = master.content.master.inode_allocated_flag[tableIndex];
    // update table
    master.content.master.inode_allocated_flag[tableIndex] = byteVal & ~(daByte >> bitNum);
    
    // write master back to memory
    if (virtual_disk_write_block(MASTER_BLOCK_REFERENCE, &master) < 0) {
      fprintf(stderr, "Fail to write master block to memory rmfile\n");
      return(-1);
    }
    
  }
  // if no n reference change then we just write the updated stuff to memory
  
  // write all back to memory
  if (virtual_disk_write_block(inode_parent.content, &block) < 0) {
    fprintf(stderr, "Fail to write parent block back to memeory");
    return(-2);
  }
  if (oufs_write_inode_by_reference(child, &inode) < 0) {
    fprintf(stderr, "Fail to write child inode back to memeory\n");
    return(-2);
  }
  if (oufs_write_inode_by_reference(parent, &inode_parent) < 0) {
    fprintf(stderr, "Fail to write parent inode back to memeory\n");
    return(-2);
  }
  // Success
  return(0);
};


/**
 * Create a hard link to a specified file
 *
 * Full implemenation:
 * - Add the new directory entry
 * - Increment inode.n_references
 *
 * @param cwd Absolute path for the current working directory
 * @param path_src Absolute or relative path of the existing file to be linked
 * @param path_dst Absolute or relative path of the new file inode to be linked
 * @return 0 if success
 *         -x if error
 * 
 */
int oufs_link(char *cwd, char *path_src, char *path_dst)
{
  INODE_REFERENCE parent_src;
  INODE_REFERENCE child_src;
  INODE_REFERENCE parent_dst;
  INODE_REFERENCE child_dst;
  char local_name[MAX_PATH_LENGTH];
  char local_name_bogus[MAX_PATH_LENGTH];
  INODE inode_src;
  INODE inode_dst;
  BLOCK block;

  // Try to find the inodes
  if(oufs_find_file(cwd, path_src, &parent_src, &child_src, local_name_bogus) < -1) {
    return(-5);
  }
  if(oufs_find_file(cwd, path_dst, &parent_dst, &child_dst, local_name) < -1) {
    return(-6);
  }

  // SRC must exist
  if(child_src == UNALLOCATED_INODE) {
    fprintf(stderr, "Source not found\n");
    return(-1);
  }

  // DST must not exist, but its parent must exist
  if(parent_dst == UNALLOCATED_INODE) {
    fprintf(stderr, "Destination parent does not exist.\n");
    return(-2);
  }
  if(child_dst != UNALLOCATED_INODE) {
    fprintf(stderr, "Destination already exists.\n");
    return(-3);
  }

  // Get the inode of the dst parent
  if(oufs_read_inode_by_reference(parent_dst, &inode_dst) != 0) {
    return(-7);
  }

  if(inode_dst.type != DIRECTORY_TYPE) {
    fprintf(stderr, "Destination parent must be a directory.");
  }
  // There must be space in the directory
  if(inode_dst.size == N_DIRECTORY_ENTRIES_PER_BLOCK) {
    fprintf(stderr, "No space in destination parent.\n");
    return(-4);
  }


  // TODO
  // create child in dst parent since condition are met

  // get directory block
  if (virtual_disk_read_block(inode_dst.content, &block) < 0) {
    fprintf(stderr, "Fail to get parent dst block\n");
    return(-1);
  }
  // find next available space in directory
  int entryIndex = 0;
  while (entryIndex < N_DIRECTORY_ENTRIES_PER_BLOCK) {
    if (block.content.directory.entry[entryIndex].inode_reference == UNALLOCATED_INODE) {
      // found spot
      break;
    }
    entryIndex++;
  }
  // add in the entry
  strcpy(block.content.directory.entry[entryIndex].name, local_name);
  block.content.directory.entry[entryIndex].inode_reference = child_src;
  // increment size
  inode_dst.size++;
  
  // entry added, now update the inode by reading it in
  if (oufs_read_inode_by_reference(child_src, &inode_src) < 0) {
    fprintf(stderr, "Fail to read in child_src inode\n");
    return(-1);
  }
  inode_src.n_references++;

  // write everything back to memory
  if (virtual_disk_write_block(inode_dst.content, &block) < 0) {
    fprintf(stderr, "Fail to write parent block to memroy flink\n");
    return(-1);
  }
  if (oufs_write_inode_by_reference(child_src, &inode_src) < 0) {
    fprintf(stderr, "Fail to write chid_src inode to memory\n");
    return(-1);
  }
  if (oufs_write_inode_by_reference(parent_dst, &inode_dst) < 0) {
    fprintf(stderr, "Fail to write parent inode to memory\n");
    return(-1);
  }
  return(0);
}




