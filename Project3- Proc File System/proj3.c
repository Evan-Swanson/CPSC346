/*
Class: CPSC 346-02
Team Member 1: Evan Swanson
Team Member 2: N/A 
GU Username of project lead: eswanson2
Pgm Name: proj3.c 
Pgm Desc: exploraiton of the proc file system 
Usage: 1) standard:  ./a.out -s 
Usage: 2) history:  ./a.out -h 
Usage: 3) load:  ./a.out -l 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void standard();
void history();
void load();

int main(int argc, char* argv[])
{
 if (argc != 2)
  {
   fprintf(stderr, "Error: Options required\n"); 
   fprintf(stderr, "usage: ./a.out -s|-h|-l\n\n"); 
   exit(EXIT_FAILURE);
  }
  
 if (!strcmp(argv[1],"-s"))
  standard();
 else if (!strcmp(argv[1],"-h"))
  history();
 else if (!strcmp(argv[1],"-l"))
  load();
 else
  printf("Error: unsupported option\n");
}
/*
pre: none
post: displays CPU vendor_id, model name, and OS version
*/
void standard()
{
 char ch;
 FILE* ifp;
 char str[80];

 /*
 I've deliberately used two different mechanisms for writing to the console.  
 Use whichever suits you.
 strstr locates a substring
 */

 ifp = fopen("/proc/cpuinfo","r");
 while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"vendor_id") || strstr(str,"model name"))
   puts(str); 
 fclose (ifp);

 ifp = fopen("/proc/version","r");
 while ((ch = getc(ifp)) != EOF)
  putchar(ch);
 fclose (ifp);
}

/*
pre: none
post: displays time since the last reboot (DD:HH:MM:SS), time when the system was last booted 
      (MM/DD/YY - HH:MM), number of processes that have been created since the last reboot.
Hint: strftime could be useful
*/
void history()
{
 FILE* inFile;
 int uptime;
 int uptimeDays;
 int uptimeHours;
 int uptimeMinutes;
 int uptimeSeconds;

 inFile = fopen("/proc/uptime","r");
 fscanf(inFile, "%d", &uptime);
 uptimeDays = uptime / 86400;
 uptimeHours = uptime / 3600 - (24 * uptimeDays);
 uptimeMinutes = uptime / 60 - (uptimeHours * 60) - (uptimeDays*24);
 uptimeSeconds = uptime - uptimeMinutes*60 - uptimeHours*3600 - uptimeDays*86400;
 printf("\nTime since last reboot: %02d:%02d:%02d:%02d\n\n", uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds);
 fclose (inFile);

 char bootTimeString[11];
 char str[80];
 char *end;
 int bootTime;
 inFile = fopen("/proc/stat", "r");
 while (fgets(str,80,inFile) != NULL)
 {
  if (strstr(str,"btime"))
  {
    int i;
    for(i = 0; i < 10; i++)
     bootTimeString[i] = str[i+6];
  }
  if(strstr(str, "processes"))
  {
   puts(str); 
  }
 }
 bootTime = strtol(bootTimeString,&end,10);
 
 char outputTime[80];
 time_t readableBootTime;
 readableBootTime = bootTime;
 struct tm *epoch_time;
 printf("Boot time: %s\n", ctime(&readableBootTime));
 
 fclose(inFile);
}

/*
pre: none
post: displays total memory, available memory, load average (avg. number of processes over the last minute) 
*/
void load()
{
 FILE* inFile;
 char str[80];
 inFile = fopen("/proc/meminfo","r");
 while (fgets(str,80,inFile) != NULL)
  if (strstr(str,"MemTotal:") || strstr(str,"MemAvailable:"))
   puts(str); 
 fclose (inFile);

 float avgLoad;
 inFile = fopen("/proc/loadavg", "r");
 fscanf(inFile, "%f", &avgLoad);
 printf("Load Average: %1f \n\n", avgLoad);
 
}
