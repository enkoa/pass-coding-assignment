/*
  BAO LE
  Project 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "storage.h"

extern void resetBuf(unsigned char buf[], int bufferSize);
extern void printBuf(unsigned char buf[], int bufferSize);
extern void inputLoop(unsigned char buf[], int bufferSize, STORAGE *storage);
  
const char SEPARATORS[] = " \t\n";

int main(int argc, char *argv[]) {
  int bufferSize = 128; // size of buffer
  unsigned char buf[bufferSize]; // the buffer
  
  STORAGE *storage = init_storage("storage.bin"); // open file
  if (storage == NULL) { // return -1 if file cannot be open
    fprintf(stderr, "Error in opening file \n");
    return(-1);
  }
  
  resetBuf(buf, bufferSize); // initialize buffer
  inputLoop(buf, bufferSize, storage); // input loop
  
  int close = close_storage(storage); // closing the file
  if (close == 0) {
    // nothing
  } else {
    fprintf(stderr, "Error closing file\n"); // error otherwise
    return(-1);
  }
  return(0);
}

void inputLoop(unsigned char buf[], int bufferSize, STORAGE *storage) {
  char in_buffer[150]; // buffer input line
  
  char choice; // commands
  int location; // location
  char val[128]; // value for commands
  
  int byteVal; // byte val for b/B
  int intVal; // int val for i/I
  char singleC; // single Char for c/C
  float floatVal; // float val for f/F
  char string[128]; // string for S not sure whether to use this or reuse in_buffer
  int len; // length of byte to be read/write for r/w
  
  int ret; // sscanf check
  char check; // extra string go here or sscanf go here
  unsigned int decimal; // for hex to decimal conversion

  char *args[3]; // all needed input will be here if valid
  char **arg; // point for that args
  while (fgets(in_buffer, 100, stdin)) {
    
    ret = sscanf(in_buffer, "%c %d %s %c", &choice, &location, &val[0], &check);

    // sscanf checks formatted input. if format match but extra argument
    // then it will be in check, and making ret > 3
    if (ret > 3) {
      fprintf(stderr, "Too much argument\n");
      continue;
    }    
    
    // if this part reached, then that mean ret <= 3, which is valid number of
    // inputs. This means tokenization is ready.

    // tokenization:
    arg = args;
    *arg++ = strtok(in_buffer, SEPARATORS);
    while ((*arg++ = strtok(NULL, SEPARATORS))) {
      // nothing while tokenization does it work
    }
    if (args[0] == NULL) {
      continue;
    }
    
    // make sure the commands is length 1
    if (strlen(args[0]) != 1) {
      fprintf(stderr, "Invalid arguments\n");
      continue;
    }

    //
    // commands starts here, aka many if's
    //
    
    if (ret == 1 && *args[0] == 'l') {
      printBuf(buf, bufferSize); // function to print
      
    } else if (ret == 1 && *args[0] == 'z') {
      resetBuf(buf, bufferSize); // function to initialize
      
    } else if (ret == 3 && *args[0] == 'b') {
      ret = sscanf(args[2], "%d", &byteVal); // scan string to decimal from args[2]
      if (ret == 1) { // if ret = 1 then successful
	memcpy(&buf[location], &byteVal, sizeof(char)); // copy to location
      } else {
	fprintf(stderr, "Invalid arguments for b.\n"); // else error
      }
      
    } else if (ret == 2 && choice == 'B') {
      memcpy(&byteVal, &buf[location], sizeof(char)); // read from location
      printf("%d\n", byteVal); // to byteVal
      
    } else if (ret == 3 && choice == 'h') {
      sscanf(args[2], "%x", &decimal); // scan string as hex to decimcal in args[2]
      memset(&buf[location], decimal, sizeof(char)); // store
      
    } else if (ret == 2 && choice == 'H') { 
      printf("%x\n", buf[location]); // printing hex is same as stored
      
    } else if (ret == 3 && choice == 'i') {
      ret = sscanf(args[2], "%d", &intVal); // scan string to int from args[2]
      if (ret == 1) { // ret = 1 means successful 
	memcpy(&buf[location], &intVal, sizeof(int)); // copy mem to location
      } else {
	fprintf(stderr, "Invalid arguments for i.\n"); // else error
      }
      
    } else if (ret == 2 && choice == 'I') {
      memcpy(&intVal, &buf[location], sizeof(int)); // copy location to intVal
      printf("%d\n", intVal); 
      
    } else if (ret == 3 && choice == 'f') {
      ret = sscanf(args[2], "%f", &floatVal); // scan string to float from args[2]
      if (ret == 1) { // ret = 1 means successful
	memcpy(&buf[location], &floatVal, sizeof(float)); // copy to location
      } else {
	fprintf(stderr, "Invalid arguments for f.\n"); // else error
      }
      
    } else if (ret == 2 && choice == 'F') {
      memcpy(&floatVal, &buf[location], sizeof(float)); // read from loc
      printf("%f\n", floatVal);
      
    } else if (ret == 3 && choice == 'c' && strlen(args[2]) == 1) {
      memcpy(&buf[location], args[2], sizeof(char)); // if args[2] length is 1
      // then it means it is a single char, so we can just read that args[2]
      // in directly
      
    } else if (ret == 2 && choice == 'C') {
      memcpy(&singleC, &buf[location], sizeof(char)); // copy loc memory to C
      printf("%c\n", singleC);
      
    } else if (ret == 3 && choice == 's') {
      memcpy(&buf[location], args[2], sizeof(char)*strlen(args[2]));
      // since args[2] will always contain a string, we can just input in
      // directly
      
    } else if (ret == 2 && choice == 'S') {
      memcpy(&string[0], &buf[location], sizeof(string)); // copy loc to string
      printf("%s\n", string);
      
    } else if (ret == 3 && choice == 'w') {
      ret = sscanf(args[2], "%d", &len); // scan int to len if possible
      if (ret == 1) { // ret = 1 means scanning to int successful
	int result = put_bytes(storage, buf, location, len); // function to write
      } else {
	fprintf(stderr, "Invalid arguments in w.\n"); // else invalid arg
      }
      
    } else if (ret == 3 && choice == 'r') {
      ret = sscanf(args[2], "%d", &len); // scan int to len if possible
      if (ret == 1) { // ret = 1 means scanning to int sucessful
	int result = get_bytes(storage, buf, location, len); // func to read
      } else {
	fprintf(stderr, "Invalid arguments in r.\n"); // else invalid arg
      }
      
      
    } else {
      fprintf(stderr, "Invalid arguments\n"); //error for generic incorrect arg
      continue;
    }

  }
}

void resetBuf(unsigned char buf[], int bufferSize) {  
  memset(&buf[0], 0, bufferSize); // set all memory from 0 to 127 position to 0
}

void printBuf(unsigned char buf[], int bufferSize) {
  int i = 0;
  for(i = 0; i < bufferSize; i++) { // loop to print out buffer
    printf("%02x ", buf[i]);
    if (i > 0 && (i+1)%16 == 0) {
      printf("\n"); // newline every 16th position
    }
  }
}
