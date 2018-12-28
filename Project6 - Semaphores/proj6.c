/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: proj6.c 
Pgm Desc: Demonstrates use of semaphores to solve the 
          producer-consumer problem
Usage: ./a.out
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_MUTEX 1 
#define INIT_EMPTY 100 
#define INIT_FULL  0 
#define EMPTY_ARG  0 //used when argument is optional 
#define PROD 0
#define CONS 1

//main() based off of ex21.c

typedef struct sembuf sem_struct;  //give it a more reasonable name

//prototypes for functions used to declutter main 
void set_sem_values(int,int,int);
void get_sem_values(int,int,int);
void set_sem_struct(sem_struct[],int);
int create_semaphore(int);
void fatal_error(char[]);
void cleanup(int,int,int);
void criticalSection(int who);
void producer(int empty, int full, int mutex, sem_struct s_wait[], sem_struct s_signal[]);
void consumer(int empty, int full, int mutex, sem_struct s_wait[], sem_struct s_signal[]);

int main(int argc, char* argv[])
{
 sem_struct s_wait[1], s_signal[1]; //create semaphore structs
 int mutex, empty, full;   //will hold semaphore identifiers 
 int value, status, ch_stat, i;

 //set wait/signal structs used in semop 
 set_sem_struct(s_wait,-1);
 set_sem_struct(s_signal,1);
 
 //create semaphore sets using arbitrary int unique to the semaphore set. 
 mutex = create_semaphore(0);
 empty = create_semaphore(1);
 full = create_semaphore(2);

 //set semaphores to initial values
 set_sem_values(mutex, empty, full);

 printf("Intial semaphore values\n");
 get_sem_values(mutex, empty, full);

 //fork off a process
 if ((value = fork()) < 0)
  fatal_error("fork");
 else
  if (value == 0)  //child
    { 
     producer(empty, full, mutex, s_wait, s_signal);
    }
  else    //parent
    { 
        consumer(empty, full, mutex, s_wait, s_signal);
    }

 if (value == 0)
   {
    sleep(7); //wait for child
    printf("Final semaphore values\n");
    get_sem_values(mutex, empty, full); 
    cleanup(mutex,empty,full); //remove semaphores
   }
 return 0;
}

//create a semaphore set of 1 semaphore specified by key 
int create_semaphore(int key)
{
 int new_sem;
 if ((new_sem = semget(key, 1, 0777 | IPC_CREAT)) == -1)
   {
    perror("semget failed");
    exit(1);
   }
 return new_sem;
}

//set struct values for down/up ops to be used in semop
//By design, we will always refer to the 0th semaphore in the set
void set_sem_struct(sem_struct sem[], int op_val)
{
 sem[0].sem_num = 0;
 sem[0].sem_op = op_val;
 sem[0].sem_flg = SEM_UNDO;
} 

//set mutex, empty, full to initial values
//these semaphore sets consist of a single semaphore, the 0th 
void set_sem_values(int mutex, int empty, int full)
{
 semctl(mutex, 0, SETVAL, INIT_MUTEX);
 semctl(empty, 0, SETVAL, INIT_EMPTY);
 semctl(full, 0, SETVAL, INIT_FULL);
}

//retrieve value held by the 0th semaphore, which is the second arg.
//0 in the final argument indicates that it is not used 
void get_sem_values(int mutex, int empty, int full)
{
 int m, e, f; 
 m = semctl(mutex, 0, GETVAL, 0);
 e = semctl(empty, 0, GETVAL, 0);
 f = semctl(full, 0, GETVAL, 0);
 printf("mutex: %d empty: %d full: %d\n", m,e,f);
}

//Display error and exit if semop fails
void fatal_error(char sem[])
{
 perror(strcat("Exiting. Failed on ",sem)); 
 exit(1);
}

//remove semaphores
void cleanup(int mutex, int empty, int full)
{
 semctl(mutex, 1, IPC_RMID, EMPTY_ARG);
 semctl(empty, 1, IPC_RMID, EMPTY_ARG);
 semctl(full, 1, IPC_RMID, EMPTY_ARG);
}

 
void producer(int empty, int full, int mutex, sem_struct s_wait[], sem_struct s_signal[])
{
  int i;
  for(i = 0; i < 5; i++)
   {
     semop(empty, s_wait, 1);
     semop(mutex, s_wait, 1);

     criticalSection(PROD);

     semop(mutex, s_signal, 1);
     semop(full, s_signal, 1);
     sleep(1);
   }

}
 
void consumer(int empty, int full, int mutex, sem_struct s_wait[], sem_struct s_signal[])
{
  int i;
  for (i = 0; i < 5; i++)
   {
    semop(full, s_wait, 1);
    semop(mutex, s_wait, 1);

    criticalSection(CONS);

    semop(mutex, s_signal, 1);
    semop(empty, s_signal, 1);
    sleep(3);
   }

}

void criticalSection(int who)
{
  if (who == PROD)
    printf("Producer making an item\n");
 else
    printf("Consumer consuming an item\n");
 }
