/*
  Bao Le
  Project 2
*/
/**
   API for remote storage.  Identical API as provided by the local storage implementation 
 */
#include <string.h>
#include "storage_remote.h"
#include "storage_common.h"
#include "comm.h"

/**
   initialize the storage

   Open the two fds (in and out), wait for an init message, initialize the local storage
 */
STORAGE * init_storage(char * name)
{
  // Create space for the STORAGE object
  STORAGE *s = malloc(sizeof(STORAGE));

  // open pipes
  fprintf(stderr, "Client: Opening pipe_in\n");
  s->fd_to_storage = open("pipe_in", O_WRONLY);
  fprintf(stderr, "Client: Opening pipe_out\n");
  s->fd_from_storage = open("pipe_out", O_RDONLY);
  fprintf(stderr, "Client: Finish opening pipes\n");
  
  if (s->fd_to_storage == -1 || s->fd_from_storage == -1) {
    fprintf(stderr, "Open error\n");
    free(s);
    return(NULL);
  }

  // pipe successfully opened

  // make header for initial connections
  HEADER h;
  h.type = INIT_CONNECTION;
  h.len_message = strlen(name) + 1;
  h.location = -1;
  h.len_buffer = -1;

  fprintf(stderr, "Client: Sending INIT_CONNECTION...\n");

  // send header
  if (write(s->fd_to_storage, &h, sizeof(h)) != sizeof(h)) {
    fprintf(stderr, "Error in writing header to the pipe from init_storage\n");
  }
  // send storage name
  if (write(s->fd_to_storage, name + '\0', h.len_message) != h.len_message) {
    fprintf(stderr, "Error in writing name to the pipe from init_storage\n");
  }
  // read from storage for acknowledge
  if (read(s->fd_from_storage, &h, sizeof(h)) != sizeof(h)) {
    fprintf(stderr, "Error in receiving ACKNOWLEDGE from the server from init_storage\n");
    free(s);
    return(NULL);
  }
  
  if (h.type != ACKNOWLEDGE) {
    fprintf(stderr, "Server didn't return ACKNOWLEDGE for init_storage\n");
    free(s);
    return(NULL);
  }
  // acknowledged received
  fprintf(stderr, "Client: INIT_CONNECTION acknowledged\n");
  
  // All okay 
  return s;
};

/**
   Shut down the connection

   Tell the server to shut down
 */
int close_storage(STORAGE *storage)
{
  // Create the shutdown message
  HEADER header;

  header.type = SHUTDOWN;
  header.len_message = 0;
  header.location = -1;
  header.len_buffer = -1;

  fprintf(stderr, "Client: Sending SHUTDOWN...\n");

  // send header
  if (write(storage->fd_to_storage, &header, sizeof(header)) != sizeof(header)) {
    fprintf(stderr, "Error in writing SHUTDOWN to the pipe from close_storage\n");
    return(-1);
  }
  // read back for acknowledge
  if (read(storage->fd_from_storage, &header, sizeof(header)) != sizeof(header)) {
    fprintf(stderr, "Error in receiving ACKNOWLEDGE from sever in close_storage\n");
    return(-1);
  }
  if (header.type != ACKNOWLEDGE) {
    fprintf(stderr, "Server didn't return ACKNOWLEDGE for close_storage\n");
    return(-1);
  }
  // acknowledge received
  fprintf(stderr, "Client: SHUTDOWN ACKNOWLEDGED\n");
  // Free the storage struction
  free(storage);

  // Done
  return(0);
}

/**
   read bytes from the storage
 */
int get_bytes(STORAGE *storage, unsigned char *buf, int location, int len)
{

  // header message
  HEADER h;
  h.type = READ_REQUEST;
  h.len_message = 0;
  h.location = location; // location
  h.len_buffer = len; // bytes length

  fprintf(stderr, "Client: Sending READ_REQUEST\n");

  // send header
  if (write(storage->fd_to_storage, &h, sizeof(h)) != sizeof(h)) {
    fprintf(stderr, "Error in writing READ_MESSAGE to pipe from get_bytes\n");
    return(-1);
  }
  // read back from server for DATA
  if (read(storage->fd_from_storage, &h, sizeof(h)) != sizeof(h)) {
    fprintf(stderr, "Error in receiving DATA for get_bytes\n");
    return(-1);
  }
  if (h.type != DATA) {
    fprintf(stderr, "Server didn't return DATA for get_bytes\n");
    return(-1);
  }

  // DATA received

  // read in data
  int ret = read(storage->fd_from_storage, buf, sizeof(char)*h.len_message);
  
  if (ret < 0) {
    fprintf(stderr, "Error in reading from pipes to buffer\n");
    return(-1);
  }
  
  fprintf(stderr, "Client: DATA sended back\n");
  // Success
  return(len);
};


/**
   Write bytes to the storae

   Send the write request message + the data
 */
int put_bytes(STORAGE *storage, unsigned char *buf, int location, int len)
{

  // header write request
  HEADER h;
  h.type = WRITE_REQUEST;
  h.len_message = len;
  h.location = location;
  h.len_buffer = len;

  fprintf(stderr, "Client: Sending WRITE_MESSAGE\n");

  // send header
  if (write(storage->fd_to_storage, &h, sizeof(h)) != sizeof(h)) {
    fprintf(stderr, "Error in writing Message to pipe from put_bytes\n");
    return(-1);
  }
  // send data
  if(write(storage->fd_to_storage, buf, sizeof(char)*len) != sizeof(char)*len) {
    fprintf(stderr, "Error in writing the buffer to pipe from put_bytes\n");
    return(-1);
  }
  // read back from server for acknowledge
  if (read(storage->fd_from_storage, &h, sizeof(h)) != sizeof(h)) {
    fprintf(stderr, "Error in receiving ACKNOWLEDGE from put_bytes\n");
    return(-1);
  }
  if (h.type != ACKNOWLEDGE) {
    fprintf(stderr, "Server didn't return ACKNOWLEDGE for put_ytes\n");
    return(-1);
  }
  // acknowledge received
  fprintf(stderr, "Client: WRITE_MESSAGE ACKNOWLEDGED\n");
  
  // Success
  return(len);
};

