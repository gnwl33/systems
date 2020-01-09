/* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#define SHMKEY 95003
#define MSGKEY 95003
#define BMSIZE 335543

typedef struct
{
  pid_t pid;
  int tested;
  int skipped;
  int found;

} pstat;

typedef struct
{
  char num[BMSIZE];
  int perfnum[20];
  pstat process[20];
  int manpid;
  int perfNum;
} shmseg;

typedef struct
{
  long type;
  int data;
} msg;