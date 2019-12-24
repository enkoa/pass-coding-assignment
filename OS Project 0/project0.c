/*
  BAO LE
  project 0
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> // probably didn't use this

extern void toEng(char string[]); // function to begin translation
extern void engWords(char string[]); // function that determine translation
extern void printString(char substring[]); // function to print translation

// zero indicate no capitalization
int caps = 0;
// single digit, zero should be blank
char *digit1[] =  {"", "one", "two","three", "four",
		  "five", "six", "seven", "eight", "nine"};
// special cases 10-19
char *digit2[] = {"ten", "eleven", "twelve", "thirteen",
		 "fourteen", "fifteen", "sixteen", "seventeen",
		 "eighteen", "nineteen"};
// all other tenths place, left space so don't have to do subtract stuff
char *digit10[] = {"", "", "twenty", "thirty", "forty", "fifty", "sixty",
		  "seventy", "eighty", "ninety"};
// terms for larger scale, short so don't need anything more 
char *scale[] = {"hundred", "thousand", "million"};

int main(int argc, char *argv[]){
  if (argc == 1) {
    // do nothing if there no input, because 0 represent lowercase
  } else if (argc == 2 && *argv[1] == 'u') { 
    caps = 1; // 1 represent uppercase
  } else {
    fprintf(stderr, "%s", "command line error\n"); // command line error
    return(-1);
  }

  char string[12]; // did 12 to be safe, 9 digit + 0 and newline.

  while (fgets(string, 11, stdin)){ // read in input while it is not EOF
    if (string[0] != '\n') {
      toEng(string); // translate
      printf("\n");
    }
  }
  
  return(0);
}

void toEng(char string[]) {
  int num = atoi (string); // turn to integers to remove zeros in front
  sprintf(string, "%d", num); // turn the integers back to string
  
  char oneK[5]; // 3 smallest digits
  char oneM[5]; // 3 middle digits
  char oneG[5]; // 3 big digits

  // split up the digit in 3 places, millions, thousands, and the rest then
  // convert back to char
  sprintf(oneK, "%d", num%1000); 
  sprintf(oneM, "%d", (num/1000)%1000);
  sprintf(oneG, "%d", num/1000000);

  if (oneG[0] != '0') { // check to see if there is a number in the millions
    engWords(oneG); // translate to english word if true
    printString(scale[2]); // call the print for the word million
  }
  if (oneM[0] != '0') { // check to see if there is a number char in 100,000's
    engWords(oneM); // translate to english word if true
    printString(scale[1]); // call print for the world thousands
  }
  if (oneK[0] != '0') { // check to see there number char under 1000
    engWords(oneK); // like above translate if true
  }
}

void engWords(char string[]) {
  // these splits the 3 digit number to individual digits
  int hundreds = atoi(string) / 100; // hundred place
  int tens = (atoi(string) % 100) / 10; // tenth place
  int ones = (atoi(string) % 100) % 10; // ones place

  if (hundreds != 0) { // if there is a hundred place
    printString(digit1[hundreds]); // print regular first digit for it
    printString(scale[0]); // print the hundred
  }
  if (tens == 1) { // check special case from 10-19 if true
    printString(digit2[ones]); // if true then send the ones place to special
    return; // char array for 10-19, return so 1's place is not printed
  } else if (tens > 1) { // else if the tens place is greater then
    printString(digit10[tens]); // print tens place fro 20-90
  }
  if (ones != 0) {
  printString(digit1[ones]); // print the ones place
  }
}

void printString(char substring[]) { // function to print
  if (caps == 1) { // deal with capitalize
    int i = 0;
    while (substring[i] != '\0') { // loop uppercase all char in substring
      printf("%c", toupper(substring[i]));
      i++;
    }
    printf(" ");
  } else { 
    printf("%s ", substring); // print normally if not u
  }
}
