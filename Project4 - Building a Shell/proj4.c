/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: proj4.c 
Pgm Desc: Shell program
Usage: 1) standard:  ls -l
Usage: 2) history:  history
Usage: 3) last command: !!
Usage: 4) 3rd command: !3
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define TRUE 80

char** getInput(int* historyIndex, char historyStrings[][MAX_LINE]);
char** parseInput(char*);
void dispOutput(char**);

int main(int argc, char* argv[])
{
  char** args;
  char done[] = "quit";
  char history[] = "history";
  char historyCommand[] = "!!";
  int value;
  int historyIndex = 0;
  char historyStrings[10][MAX_LINE];
  int executeHistoryIndex;

  while (TRUE)
  {
     printf("myShell> ");
     fflush(stdout);
     args = getInput(&historyIndex, historyStrings);
     dispOutput(args);
     
     //if the user has entered "quit" break out of the loop.
     if(strcmp(args[0], done) == 0)
        break;

     //check if display history
     else if(strcmp(args[0], history) == 0)
     {
       int i = historyIndex -1;
       for(i; i >= 0; i--)
          printf("%d: %s", i, historyStrings[i]);
     }

     //check if history command
     else if(strncmp(args[0], historyCommand, 1) == 0)
     {
       //most recent command
       if(strncmp(args[0], historyCommand, 2) == 0)
        {
          args = parseInput(historyStrings[historyIndex-1]);
          if ((value = fork()) < 0)
            printf("Child could not be created\n");
          else if (value == 0) //fork returns 0 to child process
            execvp(args[0], args);
          else
            wait(NULL); //parent wait for the child
        }

      executeHistoryIndex = args[0][1] - '0';
      args = parseInput(historyStrings[executeHistoryIndex]);

      if ((value = fork()) < 0)
        printf("Child could not be created\n");
      else if (value == 0) //fork returns 0 to child process
        execvp(args[0], args);
      else
        wait(NULL); //parent wait for the child
     }
     
     //Execute normal command
     else
     {
      if ((value = fork()) < 0)
         printf("Child could not be created\n");
      else if (value == 0) //fork returns 0 to child process
        execvp(args[0], args);
      else
        wait(NULL); //parent wait for the child
     }
  }
   return 0;
}

/*
Reads input string from the key board.   
invokes parseInput and returns the parsed input to main
*/
char** getInput(int* historyIndex, char historyStrings[][MAX_LINE])
{
  char history[] = "history\n";
  char historyCommand[] = "!!\n";
  char* userInput = calloc(MAX_LINE,sizeof(char));
  //char* userHistoryOne = calloc(MAX_LINE,sizeof(char));
  fgets(userInput, MAX_LINE, stdin);
  if(strcmp(userInput, history) != 0 && strncmp(userInput, historyCommand, 1) != 0)
  {
      strcpy(historyStrings[*historyIndex], userInput);
      *historyIndex = *historyIndex + 1;
  }
  //printf("%d", *historyIndex);
  //char word[] = "test";
  
  
  return parseInput(userInput);
}  

/*
inp is a cstring holding the keyboard input
returns an array of cstrings, each holding one of the arguments entered at
the keyboard. The structure is the same as that of argv
Here the user has entered three arguements:
myShell>cp x y
*/ 
char** parseInput(char* inp)
{
  char** parsedArray = calloc(10,sizeof(char*));
  char*  currentWord = calloc(MAX_LINE,sizeof(char));
  char*  secondWord = calloc(MAX_LINE,sizeof(char));
  char*  thirdWord = calloc(MAX_LINE,sizeof(char));
  int i = 0;
  int j = 0;
  int words = 0;


  while(inp[i] != '\0')
  {
    if(inp[i] != ' ' & inp[i] != '\n')
    {
      if(words == 0)
        currentWord[j] = inp[i];
      else if (words == 1)
        secondWord[j] = inp[i];
      else if (words == 2)
        thirdWord[j] = inp[i];
      i++;
      j++;
    }
    else
    {
      if(words == 0)
      {
        currentWord[j] = '\0';
        parsedArray[words] = currentWord;
      }
      else if (words == 1)
      {
        secondWord[j] = '\0';
        parsedArray[words] = secondWord;
      }
      else if (words == 2)
      {
        thirdWord[j] = '\0';
        parsedArray[words] = thirdWord;
      }

      j = 0;
      i++;
      words++;
    }
  }
  
  return parsedArray;
} 
 
/*
Displays the arguments entered by the user and parsed by getInput
*/
void dispOutput(char** args)
{
  int i = 0;
  while(args[i] != NULL)
  {
    printf("%s ", args[i]);
    i++;
  }
  printf("\n");
}  
