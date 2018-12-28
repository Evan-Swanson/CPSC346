/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: Proj2.c
Pgm Desc: Illustrates a common way to read from the keyboard in C 
	  as well as counts and displays the number of words entered
Usage: ./a.out
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

char* get_inp();

void disp_inp(char*);

int wc(char*);

int main()
{
 char* inp = get_inp();

 printf("%d \n", wc(inp));

 free(inp);  //return dynamially allocated memory to the heap
 return 0;
}


char* get_inp()
{
 char* inp = (char*) malloc(MAX_LINE);
 char* start = inp;
 char c;

 printf("Enter text <= 80 characters in length\n");
 while ((c = getchar()) != '\n')
  *inp++ = c; //weird, yes? First add the character to the dereffed pointer
              //then go to the next position in dynamic memory 
 *inp = '\0'; 
 return start;
}

void disp_inp(char* out)
{
 while(*out)  //continue until the null character is encountered
  putchar(*out++);
 putchar('\n');
}

/*
pre:  inp is the address of an 80 byte block of memory 
post: returns the number of words stored in the block of memory.  A word
      is any sequence of characters that 
      1) are stored at the beginning of the block or 
      2) are stored at end of the block or
      3) are terminated by the end-of-line character or
      4) are preceded by and ended by one or more spaces
        
*/
int wc(char* inp)
{
  int wordCount = 0;
  int i = 0;

  if(inp[i] != ' ' && inp[i] != '\n' && inp[i] != '\t')
     wordCount++;

  for(i ; i < MAX_LINE; i++)
  {
     if(inp[i] == ' ' || inp[i] == '\n' || inp[i] == '\t')
     {
	if(i != (MAX_LINE - 1) && inp[i+1] != ' ' && inp[i+1] != '\t' && inp[i+1] != '\0')
	  wordCount++;
     }
  }
  return wordCount;
}



