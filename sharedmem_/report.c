/* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */

#include "defs.h"

shmseg *mem;
int shmid;

int main(int argc, char *argv[])
{
  shmid = shmget(SHMKEY, sizeof(shmseg), 0);
  mem = (shmseg *)shmat(shmid, NULL, 0);

  printf("Perfect numbers: ");
  int i, j;
  for (i = 0; i < 20; i++)
  {
    if (mem->perfnum[i])
    {
      printf("%d  ", mem->perfnum[i]);
    }
  }

  printf("\n");
  printf("%-7s%-12s%-12s%-6s\n", "PID", "TESTED", "SKIPPED", "FOUND");

  for (int i = 0; i < 20; i++)
  {
    if (mem->process[i].pid)
    {
      printf("%-7d", mem->process[i].pid);
      printf("%-12d", mem->process[i].tested);
      printf("%-12d", mem->process[i].skipped);
      printf("%-6d\n", mem->process[i].found);
    }
  }
  printf("\n");

  int totTested = 0;
  int totSkipped = 0;
  int totFound = 0;

  for (int i = 0; i < 20; i++)
  {
    if (mem->process[i].pid)
    {
      totTested += mem->process[i].tested;
      totSkipped += mem->process[i].skipped;
      totFound += mem->process[i].found;
    }
  }

  printf("Total number of perfect numbers found: %d\n", totFound);
  printf("Total number of tested numbers: %d\n", totTested);
  printf("Total number of skipped numbers: %d\n\n", totSkipped);

  if (argc >= 2)
  {
    if (strcmp(argv[1], "-k") == 0)
    {
      kill(mem->manpid, SIGINT);
    }
  }
}

// #include "structs.h"

// shmseg *mem;
// int shmid;

// int main(int argc, char *argv[])
// {
//   int ret;
//   shmid = shmget(SHMKEY, sizeof(shmseg), 0);

//   mem = (shmseg *)shmat(shmid, NULL, 0);

//   printf("Perfect numbers:\n");
//   int i, j;
//   for (i = 0; i < 20; i++)
//   {
//     if (mem->perfnum[i] != 0)
//     {
//       printf("%d   ", mem->perfnum[i]);
//     }
//   }
//   printf("\n");

//   for (int i = 0; i < 20; i++)
//   {
//     if (mem->process[i].pid != 0)
//     {
//       printf("Process id: %d\n", mem->process[i].pid);
//       printf("     Number of tested numbers: %d\n", mem->process[i].tested);
//       printf("     Number of skipped numbers: %d\n\n", mem->process[i].skipped);
//       printf("     Number of perfects found: %d\n", mem->process[i].found);
//     }
//   }

//   int total_tested = 0;
//   int total_skipped = 0;
//   int total_found = 0;

//   for (int i = 0; i < 20; i++)
//   {
//     if (mem->process[i].pid != 0)
//     {
//       total_tested += mem->process[i].tested;
//       total_skipped += mem->process[i].skipped;
//       total_found += mem->process[i].found;
//     }
//   }

//   printf("Total number of perfects found: %d\n", total_found);
//   printf("Total number of tested numbers: %d\n", total_tested);
//   printf("Total number of skipped numbers: %d\n\n", total_skipped);

//   if (argc >= 2)
//   {
//     if (strcmp(argv[1], "-k") == 0)
//     {
//       shmid = shmget(SHMKEY, sizeof(shmseg), 0);
//       mem = (shmseg *)shmat(shmid, NULL, 0);

//       kill(mem->manpid, SIGINT);
//       exit(0);
//     }
//   }
//   else

//     return 0;
// }