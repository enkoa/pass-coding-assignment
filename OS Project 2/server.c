/*
  Bao Le
  Project 2
*/
#include "storage.h"
#include "comm.h"
#include "storage_common.h"
#include <string.h>

#define BUFSIZE 200

int main(int argc, char** argv)
{
  unsigned char buffer[BUFSIZE];
  HEADER header;
  HEADER header_out;
  int ret;
  STORAGE *storage;

  int fd_out;
  int fd_in;

  // Loop forever (break out with SHUTDOWN)
  while(1) {
    fprintf(stderr, "New: Waiting for connection with client...\n");

    // Open to_storage pipe    
    fprintf(stderr, "Server: Opening pipe_in\n");
    fd_in = open("pipe_in", O_RDONLY);
    fprintf(stderr, "Server: Opening pipe_out\n");
    fd_out = open("pipe_out", O_WRONLY);
    fprintf(stderr, "Server: Finish opening pipes\n");
    
    if (fd_in == -1 || fd_out == -1) {
      fprintf(stderr, "Connection failed due to pipe open errors\n");
      return(-1);
    }
    // pipes successfully open
    
    fprintf(stderr, "Server: Attempting INIT_CONNECTION\n");
    // now we attempt to read in initial connection for storage file
    if (read(fd_in, &header, sizeof(header)) != sizeof(header)) {
      fprintf(stderr, "Error in reading init_storage\n");
      return(-1);
    }
    // ifs for reading in initial connection
    if (header.type == INIT_CONNECTION) {
      // header INIT_CONNECTION RECEIVED
      if (read(fd_in, &buffer, header.len_message) != header.len_message) {
	fprintf(stderr, "Error in reading file name\n");
	return(-1);
      }
      // initialize storage
      storage = init_storage(buffer);
      if (storage == NULL) {
	fprintf(stderr, "Error in opening file\n");
	return(-1);
      }
      fprintf(stderr, "Server: Opened #%s# file \n", buffer);
    }
    // storage established, we can return acknowledge now
    header_out.type = ACKNOWLEDGE;
    header_out.len_message = 0;
    header_out.location = -1;
    header_out.len_buffer = -1;
    if (write(fd_out, &header_out, sizeof(header_out)) != sizeof(header_out)) {
      fprintf(stderr, "Error in returning initial ACKNOWLEDGE\n");
      return(-1);
    }
    fprintf(stderr, "Server: INIT_CONNECTION ACKNOWLEDGED\n");
    // storage is made, connection is done
    
    // loop to read read, write, and shutdown
    while (1) {
      // first read in the header
      if (read(fd_in, &header, sizeof(header)) != sizeof(header)) {
	fprintf(stderr, "Error in reading HEADER\n");
	return(-1);
      }

      // now interpret header

      if (header.type == SHUTDOWN) {
	// shutdown
	// header for ACKNOWLEDGE
	header_out.type = ACKNOWLEDGE;
	header_out.len_message = 0;
	header_out.location = -1;
	header_out.len_buffer = -1;
	// send ACKNOWLEDGE
	if (write(fd_out, &header_out, sizeof(header_out)) != sizeof(header_out)) {
	  fprintf(stderr, "Error in writing ACKNOWLEDGE for SHUTDOWN\n");
	  return(-1);
	}
	// ACKNOWLEDGE sent
	fprintf(stderr, "Server: SHUTDOWN ACKNOWLEDGED\n");
	// sleep then break out of loop
	sleep(1);
	break;
	
      } else if (header.type == READ_REQUEST) {
	// read request
	fprintf(stderr, "Server: Beginning READ_REQUEST\n");
	// get the requested byte into the buffer
	ret = get_bytes(storage, buffer, header.location, header.len_buffer);

	// check for successful
	if (ret >= 0) {
	  // header for DATA
	  header_out.type = DATA;
	  header_out.len_message = ret; // the length of returning data is same as # read
	  header_out.location = -1;
	  header_out.len_buffer = -1;

	  // send DATA header
	  if (write(fd_out, &header_out, sizeof(header_out)) != sizeof(header_out)) {
	    fprintf(stderr, "Error in writing DATA to pipe from READ_REQUEST\n");
	    return(-1);
	  }
	  // send the actual data
	  if (write(fd_out, buffer, sizeof(char)*ret) != sizeof(char)*ret) {
	    fprintf(stderr, "Error in writing buffer to pipe from READ_REQUEST\n");
	    return(-1);
	  }
	}
	// read will alway be sucessful but this print out actualy byte read
	fprintf(stderr, "Server: Read %d bytes successfully\n", ret);
	  
	
      } else if (header.type == WRITE_REQUEST) {
	// write request
	fprintf(stderr, "Server: Beginning WRITE_REQUEST...\n");
	// in this case both len_message and len_buffer is same so no worry

	// read in the data to write to buffer
	if (read(fd_in, buffer, header.len_message) != header.len_message) {
	  fprintf(stderr, "Error in reading in buffer for WRITE_REQUEST\n");
	  return(-1);
	}
	// fprintf(stderr,"Just read in: %s\n", buffer);

	// put the data from buffer to the storage
	ret = put_bytes(storage, buffer, header.location, header.len_buffer);

	// check to see if data successfully wrote
	if (ret == header.len_buffer) {
	  // ACKNOLWEDGE header
	  header_out.type = ACKNOWLEDGE;
	  header_out.len_message = 0;
	  header_out.location = -1;
	  header_out.len_buffer = -1;

	  // send to pipe ACKNOWLEDGE header
	  if (write(fd_out, &header_out, sizeof(header_out)) != sizeof(header_out)) {
	    fprintf(stderr, "Error in sending ACKNOWLEDGE for WRITE_REQUEST\n");
	    return(-1);
	  }
	  // write succeeded in going to pipe
	  fprintf(stderr, "Server: Write %d bytes successfully\n", header.len_message);
	} else {
	  fprintf(stderr, "Error in writing to file ret != len\n");
	  return(-1);
	}
	
      } else {
	// this should never be reached
	fprintf(stderr, "Oops, there seems to be a programmer's mistake...\n");
	return(-1);
	
      }
    }

    // We broke out because of a disconnection: clean up
    fprintf(stderr, "Closing connection\n\n");
    close(fd_in);
    close(fd_out);
    close_storage(storage);
  }

  // Should never reach here
  return(0);
}
