/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: proj7.c 
Pgm Desc: Demonstrates use a pipe and threads to solve
          the reader-writer problem using prime numbers
Usage: ./a.out 100
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

void* writer(void*);
void* f2(void*);
int is_prime(int);

struct params
{
 int num;
 int id;
 int* p;
};

typedef struct params param;

int main(int argc, char* argv[])
{
    pthread_t t1,t2,t3,t4,t5;
    param p1,p2,p3,p4,p5;
    int n, status;
    int p[2];

    //random seed
    srand ( time(NULL) );

    //check input
    if (argc != 2)
    {
        printf("Error: Incorrect command line arguments. Recommended Ex: ./a.out 100\n");
        exit(1);
    }
    n = atoi(argv[1]);

    pipe(p);
    
    //create writer thread
    p1.num = n;
    p1.p = p;
    status = pthread_create(&t1, NULL, writer, (void*)&p1 );
    if (status != 0)
    {
        printf("Error in thread 1:  %d\n",status);
    }

    //create 4 reader threads
    p2.num = n;
    p2.p = p;
    p2.id = 1;
    status = pthread_create(&t2, NULL, f2, (void*)&p2 );

    p3.p = p;
    p3.id = 2;
    status = pthread_create(&t3, NULL, f2, (void*)&p3 );

    p4.p = p;
    p4.id = 3;
    status = pthread_create(&t4, NULL, f2, (void*)&p4 );

    p5.p = p;
    p5.id = 4;
    status = pthread_create(&t5, NULL, f2, (void*)&p5 );

    //join threads
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);
    return 0; 
}

//pre: paramm_in variables have been assigned
//post: pipe has n random numbers written to it
void* writer(void* param_in) 
{
    param* par = (param*)param_in;
    int ct = 0;
    int cand;

    //write large numbers to pipe
    while (ct < par->num)
    {
        cand = (rand() % (1000000 - 1)) + 1;   
        printf("Generated: %d\n",cand);
        write(par->p[1], &cand, sizeof(int));
        ct++;
    }
    close(par->p[1]);
    pthread_exit(0);
}

int is_prime(int num)
{
 int i = 2;
 while (i < num)
 {
  if (num % i == 0)
     return 0;
  ++i;
 }
 return 1;
} 

//pre: param_in variables have been assinged
//post: outputs thread id and prime number if found
void* f2(void* param_in) 
{
    int currentNumber;
    param* par = (param*)param_in;
    while(read(par->p[0], &currentNumber, sizeof(int)) > 0)
    {
        if(is_prime(currentNumber))
            printf("Reader%d:  %d is prime\n", par->id, currentNumber);
    }
    sleep(1);
    pthread_exit(0);
} 
