/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: proj8.c 
Pgm Desc: Demonstrates simulated virtual memory and demand paging
Usage: ./a.out BACKING_STORE.bin addresses.txt
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  //allows use of bool type

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE PAGES * PAGE_SIZE 




//single entry in the page table.  present_absent = 'A' page not in RAM.  
//present_absent = 'P' page in RAM
struct pagetable_entry 
   {
    char present_absent; 
    int frame_number;
   };
typedef struct pagetable_entry pagetable_entry; 


//decomposed entry from addresses.txt
struct virt_addr 
   {
    int page_number;
    int offset; 
   };
typedef struct virt_addr virt_addr;  

//file references
struct files
{
  char* backing;         //reference to secondary storage (BACKING_STORE.bin)
  FILE* fin;             //reference to address bus (addresses.txt)
};
typedef struct files files;
  
//function prototypes
void init_pagetable(pagetable_entry*);
files open_files(char*, char*);
void  translate(files, virt_addr*, int*);
char write_read_memory(char*,  virt_addr*, files, int*, int* );
bool check_pagetable(pagetable_entry*,virt_addr,int*);
void display_values(int, int, char);

int main(int argc, char *argv[])
{ 
  int logical_address;  	       //address from addresses.txt
  virt_addr virtual_address;           //holds page number and offset from logical_address
  int physical_address;                //offset into page frame 
  pagetable_entry pagetable[PAGES];    //page table
  char main_memory[MEMORY_SIZE];       //RAM
  files file_ids;                      //identifiers for the two files used in the program
  int frame_number;

  init_pagetable(pagetable);

  file_ids = open_files(argv[1],argv[2]);
  int frame_counter = 0;
  char value;
  double pageFaults;
  int numAddresses;


  //********************//

  while(feof(file_ids.fin) == 0)
  {
    numAddresses++;
    translate(file_ids, &virtual_address, &logical_address);
    if(check_pagetable(pagetable, virtual_address, &frame_number))
    {
      physical_address = (frame_number << OFFSET_BITS) | virtual_address.offset; 
      value = main_memory[frame_number * PAGE_SIZE + virtual_address.offset];
    }

    else
    {
      pageFaults++;
      value = write_read_memory(main_memory, &virtual_address,file_ids,&physical_address, &frame_counter);
      pagetable[virtual_address.page_number].present_absent = 'P';
      pagetable[virtual_address.page_number].frame_number = frame_counter;
      frame_counter++;
    }
      
    display_values(logical_address, physical_address, value);
  }

  double rate = pageFaults/numAddresses;
  printf("Number of Translated Addresses: %d \n", numAddresses);
  printf("Page Fault Rate: %.3f\n", rate);
  return 0;
}

//Init page table.  Set absent field to 'A'
void init_pagetable(pagetable_entry* pagetable)
{
 for (int i = 0; i < PAGES; i++)
       pagetable[i].present_absent = 'A';
} 

//open and prepare files
files open_files(char* argv_1, char* argv_2)
{ 
  files ids;

  //allow BACKING_STORE.bin to be treated as an array
  int backing_fd = open(argv_1, O_RDONLY);  //open BACKING_STORE.bin
  ids.backing = mmap(0,MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0);

  ids.fin = fopen(argv_2,"r"); //open addresses.txt
  return ids;

}

//read and translate address 
void translate(files ids, virt_addr* virt_address, int* logical_address_out)
{
 int logical_address;    //the value stored in address.txt 
 fscanf(ids.fin,"%d", &logical_address);
 virt_address->page_number = (logical_address >> OFFSET_BITS) & PAGE_MASK;
 virt_address->offset = logical_address & OFFSET_MASK;
 *logical_address_out = logical_address;
}

//copy backing store to main memory
//read sought after value from offset into memory
char write_read_memory(char* main_memory,  virt_addr* virtual_address, files file_ids, 
		int* physical_address, int* frame_counter)
{
 memcpy(main_memory + *frame_counter * PAGE_SIZE,
		 file_ids.backing + virtual_address->page_number * PAGE_SIZE, PAGE_SIZE);


 //offset into page frame 
 *physical_address = (*frame_counter << OFFSET_BITS) | virtual_address->offset; 
 char value = main_memory[*frame_counter * PAGE_SIZE + virtual_address->offset];
 return value;
}

//interrogate page table
bool check_pagetable(pagetable_entry* pagetable, virt_addr virtual_address, int* frame_number)
{
 if (pagetable[virtual_address.page_number].present_absent == 'A')
   return false;
 *frame_number = pagetable[virtual_address.page_number].frame_number;
 return true; 
}


void display_values(int logical_address, int physical_address, char value)
{
 printf("Virtual address: %d Physical address: %d Value: %d\n",
		 logical_address, physical_address, value);
}
