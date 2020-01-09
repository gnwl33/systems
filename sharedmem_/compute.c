/* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */

#include "defs.h"

shmseg *mem;
int shmid;
int msgid;
int pInd;

int checkPerfect(int num)
{
  int sum = 0;
  for (int i = 1; i < num; i++)
  {
    if (num % i == 0)
    {
      sum += i;
    }
  }
  if (sum == num)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void terminate(int signum)
{
  if (pInd < 0)
    exit(0);
  mem->process[pInd].pid = 0;
  mem->process[pInd].found = 0;
  mem->process[pInd].tested = 0;
  mem->process[pInd].skipped = 0;
  exit(0);
}

int main(int argc, char *argv[])
{
  int begin;
  if (argc != 2)
  {
    begin = 2;
  }
  else
  {
    begin = atoi(argv[1]);
  }
  int cand = begin;

  shmid = shmget(SHMKEY, sizeof(shmseg), 0);
  if (shmid == -1)
  {
    printf("shmget error");
    exit(1);
  }
  mem = shmat(shmid, NULL, 0);
  if (mem == (void *)-1)
  {
    printf("shmat error");
    exit(1);
  }

  msgid = msgget(MSGKEY, 0);
  if (msgid == -1)
  {
    printf("msgget error");
    exit(1);
  }

  struct sigaction sigac;
  sigac.sa_handler = terminate;
  sigaction(SIGINT, &sigac, NULL);
  sigaction(SIGQUIT, &sigac, NULL);
  sigaction(SIGHUP, &sigac, NULL);

  msg *msg = malloc(sizeof(msg));
  msg->data = getpid();
  msg->type = 2;
  msgsnd(msgid, msg, sizeof(msg->data), 0);
  msgrcv(msgid, msg, sizeof(msg->data), 3, 0);
  pInd = msg->data;

  int stop = -1;
  while (cand != stop)
  {
    if (!(mem->num[cand]))
    {
      if (checkPerfect(cand))
      {
        msg->data = cand;
        msg->type = 1;
        msgsnd(msgid, msg, sizeof(msg->data), 0);
        mem->process[pInd].found++;
      }
      mem->num[cand] = 1;
      mem->process[pInd].tested++;
    }
    else
    {
      mem->process[pInd].skipped++;
    }

    cand++;
    if (cand == BMSIZE)
    {
      cand = 2;
      stop = begin;
    }
  }
  execl("report", "report", "-k", NULL);
}

// #include "structs.h"

// shmseg *mem;
// int shmid;
// int msgid;
// int pInd;

// void terminate(int signum)
// {
//   mem->process[pInd].pid = 0;
//   mem->process[pInd].found = 0;
//   mem->process[pInd].tested = 0;
//   mem->process[pInd].skipped = 0;
//   printf("Terminated");
//   exit(0);
// }

// int checkPerfect(num)
// {
//   int sum = 0;
//   for (int i = 2; i < num; i++)
//   {
//     if (num % i == 0)
//     {
//       sum += i;
//     }
//   }
//   if (sum == num)
//   {
//     return 1;
//   }
//   else
//   {
//     return 0;
//   }
// }

// int main(int argc, char *argv[])
// {
//   printf("hello compute");
//   shmid = shmget(SHMKEY, sizeof(shmseg), 0);
//   if (shmid == -1)
//   {
//     perror("Error shmget: shmget failed to allocate shared memor; check if manage is running.(./manage)");
//     exit(-1);
//   }
//   mem = shmat(shmid, NULL, 0);
//   if (mem == (void *)-1)
//   {
//     perror("Mem error");
//     exit(-1);
//   }

//   struct sigaction action;
//   action.sa_handler = terminate;

//   sigaction(SIGINT, &action, NULL);
//   sigaction(SIGQUIT, &action, NULL);
//   sigaction(SIGHUP, &action, NULL);

//   msgid = msgget(MSGKEY, 0);
//   if (mem == (void *)-1)
//   {
//     perror("Msg error");
//     exit(-1);
//   }
//   printf("Hello 1");
//   msg *message = malloc(sizeof(msg));
//   message->type = 2;
//   message->data = getpid();
//   printf("Hello 2");

//   int ms = msgsnd(msgid, message, sizeof(message->data), 0);

//   msgrcv(msgid, message, sizeof(message->data), 4, 0);
//   printf("Hello receive");
//   pInd = message->data;

//   int start;
//   if (argc == 2)
//   {
//     start = atoi(argv[1]);
//     printf("argv[1]");
//   }
//   else
//   {
//     start = 2;
//   }
//   int cand = start;

//   while (1)
//   {
//     printf("In while");
//     int segInd = (cand - 2) / 32;
//     int bitInd = (cand - 2) % 32;

//     if (!(mem->bitmap[segInd] & (1 << bitInd)))
//     {
//       printf("In while1");
//       if (checkPerfect(cand))
//       {
//         printf("In while2");
//         message->data = cand;
//         message->type = 1;
//         msgsnd(shmid, message, sizeof(message->data), 0);
//         mem->process[pInd].found++;
//       }
//       mem->bitmap[segInd] |= (1 << bitInd);
//       mem->process[pInd].tested++;
//     }
//     else
//     {
//       printf("Else");
//       mem->process[pInd].skipped++;
//     }

//     cand++;
//   }
// }