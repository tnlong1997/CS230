#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "words.h"

#define MAXBUF 500

/**
 * The make_string function will allocate a new C string given a
 * buffer of characters and its length. It is the responsibility of
 * the caller of this function to deallocate the string.
 */
static char *make_string(char buffer[], int length) {
  char *str = (char *)(malloc(length+1));
  memcpy(str, buffer, length);
  str[length] = '\0';
  return str;
}

/**
 * The words_next_word function returns the next word from standard
 * input. A word is a sequence of characters that are A-Z, a-z, or
 * 0-9. Any other character is a non-word character and is used to
 * delimit words.
 */
char *words_next_word() {
  char buf[MAXBUF];
  // TODO: The implementation of this function is simple. You must use
  // the getchar() function (as covered in class) to retrieve the next
  // character from standard input. If the character is an
  // alphanumeric character (a-z,A-Z,0-9) then it is a word character
  // and should be added to the character buffer `buf`. You keep
  // iterating, reading characters from standard input, until you
  // reach a non-word character. If you do you must call the
  // `make_string` function to create a new string from your character
  // buffer `buf` and return the new string as a result of this
  // function.
  //
  // HINTS: 
  //  (1) Pay attention to leading non-word characters, i.e., skip any non-word
  //      characters before accumulating word characters
  //  (2) Handle EOF and the last word correctly.
  //  (3) Read the result of getchar() into an int, for proper EOF testing
  /*char c;
  int n = 0;
  int t = 0;
  do {
    t = getchar();
  } while (!isalnum(t));

  do {
    if (isalnum(t)) {
      buf[n] = t;
      n++;
    } 
    t = getchar();
  } while (isalnum(t));

  return make_string(buf,n);*/
  //char buf[MAXBUF];
  int current_size = 0;
  int c;
  int check = 1;
  // TODO: The implementation of this function is simple. You must use
  // the getchar() function (as covered in class) to retrieve the next
  // character from standard input. If the character is an
  // alphanumeric character (a-z,A-Z,0-9) then it is a word character
  // and should be added to the character buffer `buf`. You keep
  // iterating, reading characters from standard input, until you
  // reach a non-word character. If you do you must call the
  // `make_string` function to create a new string from your character
  // buffer `buf` and return the new string as a result of this
  // function.
  //
  // HINTS: 
  //  (1) Pay attention to leading non-word characters, i.e., skip any non-word
  //      characters before accumulating word characters
  //  (2) Handle EOF and the last word correctly.
  //  (3) Read the result of getchar() into an int, for proper EOF testing
  while(check){ //start the first loop to get to the alphanumeric position
    c = getchar(); // get next char to run loop
    if(isalnum(c)) break; //stop the loop when got the position of the first alphanumeric postion
  }

  while(check){ //start the second loop at the alphanumeric position
    if(!isalnum(c)) break; //base case: if get the non-alphanumeric then break to get that position
    else buf[current_size++] = c; //put to buffer
    c = getchar(); //get next char to run loop
  }
  return make_string(buf, current_size); //return the new string
}
