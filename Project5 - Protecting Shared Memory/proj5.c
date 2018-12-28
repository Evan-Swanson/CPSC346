/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: proj5.c 
Pgm Desc: Peterson solution implementation using shared memory
Usage: 1) standard:  ./a.out
Usage: 2) user specified: ./a.out 1 2 0 1
*/

#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void parent(int time_crit_sect, int time_non_crit_sect,  int* pr_0Address, int* pr_1Address, int* turnAddress);
void child(int time_crit_sect, int time_non_crit_sect, int* pr_0Address, int* pr_1Address, int* turnAddress);
void cs(char process, int time_crit_sect);
void non_crit_sect(int time_non_crit_sect);

void main(int argc, char* argv[])
{
  int time_child = 2;
  int time_child_non_cs = 2;
  int time_parent = 0;
  int time_parent_non_cs = 1;

  //create shared memory
  int turn = shmget(0,1,0777 | IPC_CREAT);
  int* turnAddress;
  turnAddress = (int*)shmat(turn,0,0); 

  int pr_0 = shmget(0,1,0777 | IPC_CREAT);
  int* pr_0Address;
  pr_0Address = (int*)shmat(pr_0,0,0);
  *pr_0Address = 0;

  int pr_1 = shmget(0,1,0777 | IPC_CREAT);
  int* pr_1Address;
  pr_1Address = (int*)shmat(pr_1,0,0); 
  *pr_1Address = 0;


  //check for proper arguments
  if(argc != 1 && argc != 5)
  {
    printf("Error: Incorrect number of command line arguments \n");
    exit(1);
  }

  //set custom sleep times
  else if (argc == 5)
  {
   time_parent = *argv[1] - '0';
   time_child = *argv[2] - '0';
   time_parent_non_cs = *argv[3] - '0';
   time_child_non_cs = *argv[4] - '0';
  }
 
  //fork here
  if (fork() == 0)
  {
    child(time_child, time_child_non_cs, pr_0Address, pr_1Address, turnAddress);
    wait(NULL);

    //detach shared memory from the process
    shmdt(turnAddress);
    shmdt(pr_0Address);
    shmdt(pr_1Address);
  }
  else
  {
    parent(time_parent, time_parent_non_cs, pr_0Address, pr_1Address, turnAddress);
    wait(NULL);

    //detach shared memory from the process
    shmdt(turnAddress);
    shmdt(pr_0Address);
    shmdt(pr_1Address);

    //remove it 
    shmctl(turn,IPC_RMID,0);
    shmctl(pr_1,IPC_RMID,0);
    shmctl(pr_0,IPC_RMID,0);
  }

}

void parent(int time_crit_sect, int time_non_crit_sect,  int* pr_0Address, int* pr_1Address, int* turnAddress)
{
 for (int i = 0; i < 10; i++)
  {
   //protect
   *pr_1Address = 1;
   *turnAddress = 0;
   while(*pr_0Address && !*turnAddress); //do nothing

   cs('p', time_crit_sect);
   *pr_1Address = 0;
   non_crit_sect(time_non_crit_sect); 
  }
}

void child(int time_crit_sect, int time_non_crit_sect, int* pr_0Address, int* pr_1Address, int* turnAddress)
{
 for (int i = 0; i < 10; i++)
  {
   //protect
   *pr_0Address = 1;
   *turnAddress = 1;
   while(*pr_1Address && *turnAddress); //do nothing
   
   cs('c', time_crit_sect);
   *pr_0Address = 0;
   non_crit_sect(time_non_crit_sect); 
  }
}

void cs(char process, int time_crit_sect)
{
 if (process == 'p')
  {
   printf("parent in critical sction\n");
   sleep(time_crit_sect);
   printf("parent leaving critical section\n");
  }
 else
  {
   printf("child in critical section\n");
   sleep(time_crit_sect);
   printf("child leaving critical section\n");
  }
}

void non_crit_sect(int time_non_crit_sect)
{
 sleep(time_non_crit_sect);
}
