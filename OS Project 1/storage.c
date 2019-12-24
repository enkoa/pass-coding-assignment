/*
  BAO LE
  Project 1
*/
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "storage.h" // will never know if i needed this
#include <unistd.h>

STORAGE *init_storage(char *name) {
  STORAGE *storage = (STORAGE*)malloc(sizeof(STORAGE)); // allocate for STORAGE
  storage->fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // open file
  if (storage->fd == -1) {
    free(storage); // free space if file descriptor return -1
    storage=NULL; // set storage to null
  }
  return(storage); // return storage pointer
}

int close_storage(STORAGE *storage) {
  int checkClose = close(storage->fd); // basically ret on closing
  if (checkClose == 0) { // if 0 is return then close successful
    free(storage); // free space
    storage = NULL; // set pointer to null
    return(0); // return 0
  } else {
    return(-1); // return -1 if failed
  }
}

int get_bytes(STORAGE *storage, unsigned char *buf, int location, int len) {
  int ret;
  ret = lseek(storage->fd, location, SEEK_SET); // set file offset

  if (ret == -1) { 
    fprintf(stderr, "lseek error\n"); // lseek fail if ret is -1
    return(-1);
  }

  // if ret != -1
  ret = read(storage->fd, buf, sizeof(char)*len); // attempt to read 

  if (ret < 0) {
    fprintf(stderr, "reading error\n"); // reading error if ret is negative
    return(-1);
  } else if (ret == 0) { 
    fprintf(stderr, "EOF\n"); // case for End of File
    return(0);
  } else {
    return(ret); // otherwise return number of byte read
  }
}

int put_bytes(STORAGE *storage, unsigned char *buf, int location, int len) {
  int ret;
  ret = lseek(storage->fd, location, SEEK_SET); // set file offset

  if (ret == -1) {
    fprintf(stderr, "lseek error\n"); // if ret is negative then lseek error
    return(-1);
  }

  ret = write(storage->fd, buf, sizeof(char)*len); // attempt to write
  
  if (ret == len) { // if the amount to write is equal to the amount written
    return(len); // return that number, for it succeed
  } else {
    fprintf(stderr, "Error on writing\n"); // if not all byte is written
    return(-1); // then return -1 for error
  }
}
