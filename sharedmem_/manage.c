/* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */

#include "defs.h"

shmseg *mem;
int shmid;
int msgid;

void init()
{
  memset(mem, 0, sizeof(shmseg));
  mem->manpid = getpid();
  printf("Initialized memory\n");
}

void killsig(int signum)
{
  int i;
  printf("Kill signal\n");

  for (i = 0; i < 20; i++)
  {
    if (mem->process[i].pid)
    {
      kill(mem->process[i].pid, SIGINT);
    }
  }

  sleep(5);
  shmdt(mem);
  shmctl(shmid, IPC_RMID, NULL);
  msgctl(msgid, IPC_RMID, NULL);

  exit(0);
}

int main(int argc, char *argv[])
{
  msg *msg;

  shmid = shmget(SHMKEY, sizeof(shmseg), IPC_CREAT | IPC_EXCL | 0666);
  mem = shmat(shmid, NULL, 0);
  msgid = msgget(MSGKEY, IPC_CREAT | IPC_EXCL | 0666);

  if (shmid == -1)
  {
    printf("shmget error: %s\n", strerror(errno));
    exit(1);
  }
  if (mem == (void *)-1)
  {
    printf("shmat error");
    exit(1);
  }
  if (msgid == -1)
  {
    printf("msgget error");
    exit(1);
  }

  init();

  struct sigaction sigac;
  sigac.sa_handler = killsig;
  if (sigaction(SIGINT, &sigac, NULL) == -1)
  {
    printf("SIGINT error\n");
    exit(1);
  }
  if (sigaction(SIGQUIT, &sigac, NULL) == -1)
  {
    printf("SIGQUIT error\n");
    exit(1);
  }
  if (sigaction(SIGHUP, &sigac, NULL) == -1)
  {
    printf("SIGHUP error\n");
    exit(1);
  }

  int i;
  int pInd, perfInd = 0, fnd;
  while (1)
  {
    msg = malloc(sizeof(msg));
    msgrcv(msgid, msg, sizeof(msg->data), -2, 0);

    if (msg->type == 1)
    {
      fnd = 0;
      for (i = 0; i < 20; i++)
      {
        if (mem->perfnum[i] == msg->data)
        {
          fnd = 1;
          break;
        }
      }
      if (fnd == 0)
      {
        if (mem->perfNum >= 20)
        {
          printf("Already 20 perf num");
          mem->perfNum--;
        }
        mem->perfnum[mem->perfNum] = msg->data;
        mem->perfNum++;
      }
    }

    else if (msg->type == 2)
    {
      pInd = -1;
      for (i = 0; i < 20; i++)
      {
        if (mem->process[i].pid == 0)
        {
          mem->process[i].pid = msg->data;
          pInd = i;
          break;
        }
      }

      msg->type = 3;
      msg->data = pInd;
      msgsnd(msgid, msg, sizeof(msg->data), 0);
    }
  }
}

// #include "structs.h"

// shmseg *mem;
// int shmid;
// int msgid;

// void killsig(int signum)
// {

//   int i;
//   // shmid = shmget(SHMKEY, sizeof(shmseg), 0);
//   // if (shmid == -1)
//   // {
//   //   printf("Failed to allocate shared mem: %s\n", strerror(errno));
//   //   exit(1);
//   // }

//   printf("Kill signal\n");

//   for (i = 0; i < 20; i++)
//   {
//     if (mem->process[i].pid != 0)
//     {
//       kill(mem->process[i].pid, SIGKILL);
//       mem->process[i].pid = 0;
//     }
//     else
//     {
//       break;
//     }
//   }

//   sleep(5);
//   printf("hello");
//   shmdt(mem);
//   shmctl(shmid, IPC_RMID, NULL);
//   msgctl(msgid, IPC_RMID, NULL);

//   exit(0);
// }

// void initialize()
// {
//   int i;
//   for (i = 0; i < 20000; i++)
//   {
//     mem->bitmap[i] = 0;
//   }

//   for (i = 0; i < 20; i++)
//   {
//     mem->process[i].pid = 0;
//     mem->process[i].found = 0;
//     mem->process[i].tested = 0;
//     mem->process[i].skipped = 0;
//   }

//   for (i = 0; i < 20; i++)
//   {
//     mem->perfnum[i] = 0;
//   }

//   mem->perffound = 0;
//   mem->manpid = getpid();
//   printf("\nInitialized mem\n");
// }

// int main(int argc, char *argv[])
// {
//   printf("hello manage");
//   msg message;

//   shmid = shmget(SHMKEY, sizeof(shmseg), IPC_CREAT | IPC_EXCL | 0666);
//   mem = shmat(shmid, NULL, 0);

//   if (shmid == -1)
//   {
//     printf("Failed to allocate shared mem: %s\n", strerror(errno));
//     exit(1);
//   }
//   if (mem == (void *)-1)
//   {
//     perror("Mem error");
//     exit(-1);
//   }

//   initialize();

//   msgid = msgget(MSGKEY, 0);

//   struct sigaction sigac;

//   sigac.sa_handler = killsig;
//   sigaction(SIGINT, &sigac, NULL);
//   sigaction(SIGQUIT, &sigac, NULL);
//   sigaction(SIGHUP, &sigac, NULL);

//   int i;
//   int flag;
//   while (1)
//   {
//     msgrcv(msgid, &message, sizeof(message.data), -3, 0);
//     if (message.type == 1)
//     {
//       flag = 0;
//       for (i = 0; i < 20; i++)
//       {
//         if (mem->perfnum[i] == message.data)
//         {

//           flag = 1;
//           break;
//         }
//       }
//       if (!flag)
//       {
//         if (mem->perffound >= 20)
//         {
//           perror("Too much ");
//           mem->perffound--;
//         }
//         mem->perfnum[mem->perffound] = message.data;
//         mem->perffound++;
//       }
//     }
//     else if (message.data == 1)
//     {
//       flag = 0;
//       for (i = 0; i < 20; i++)
//       {
//         if (mem->perfnum[i] == message.data)
//         {
//           printf("Flag1");
//           flag = 1;
//           break;
//         }
//       }
//       if (!flag)
//       {
//         if (mem->perffound >= 20)
//         {
//           perror("Too much ");
//           mem->perffound--;
//         }
//         mem->perfnum[mem->perffound] = message.data;
//         mem->perffound++;
//       }
//     }
//     else
//       break;
//   }
// }