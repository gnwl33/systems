/* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

void sort(int psfd[2], int ssfd[2]){
  pid_t child;
  if ((child = fork()) == -1){ //create child process
    fprintf(stderr, "Failed to sort\n");
    exit(1);
  }
  else if(child == 0){ //this is the child
    close(psfd[1]); //close unneeded pipe ends
    close(ssfd[0]);

    dup2(psfd[0],0); //connect 0 (stdin) to PS pipe read end
    close(psfd[0]); //close previous fd

    dup2(ssfd[1],1); //connect 1 (stdout) to SS pipe write end
    close(ssfd[1]); //close previous fd

    execlp("sort", "sort", NULL); //execute sort using PATH
    exit(1); //shouldn't get here unless error
  }
}

void suppress(int psfd[2], int ssfd[2]){
  pid_t child;
  if ((child = fork()) == -1){ //create child process
    fprintf(stderr, "Failed to suppress\n");
    exit(1);
  }
  else if (child == 0){ //this is the child
    close(psfd[1]); //close unneeded pipe ends
    close(psfd[0]);
    close(ssfd[1]);

    FILE* stream1 = fdopen(ssfd[0],"r"); //set stream to read end of SS pipe
    char current[34]; //store current word
    char next[34]; //store next word
    int mult=1; //keep track of multiplicity

    fgets(current, 34, stream1);
    while(fgets(next, 34, stream1) != NULL){
      if (strcmp(current, next) == 0){
        mult++; //increase mult if words are the same
        continue;
      }
      printf("%-5d%s", mult, current); //print when word is unique
      strcpy(current,next); //store next as current
      memset(next, '\0', 34); //reset all values to null in next
      mult = 1; //reset mult
    }
    printf("%-5d%s", mult, current); //last word
    fclose(stream1);
    exit(0);
  }
}

int main (int argc, char* argv[]){
  int psfd[2], ssfd[2]; //pipe fd for parse-sort & for sort-suppress

  if (pipe(psfd) == -1){
    fprintf (stderr, "Parse-Sort pipe failure\n");
    exit(1);
  }

  if (pipe(ssfd) == -1){
    fprintf (stderr, "Sort-Suppress pipe failure\n");
    exit(1);
  }

  sort(psfd, ssfd);

  suppress(psfd, ssfd);

  /* Parse */
  FILE* stream = fdopen(psfd[1],"w"); //write to PS pipe
  char buf[34]={'\0'}; //initialize as local array of null values; 32 letters + \n + \0
  int length = 0; //count length of word
  char c;

  while((c = fgetc(stdin)) != EOF){ //get char until end of file
    if(isalpha(c)){ //if c is a letter
      if (length < 32){ //max length is 32
        buf[length++] = tolower(c); //all letters are lowercase
      }
    }
    else{ //non-letter char reached
      if(length >= 5){ //min length is 5
        buf[length++] = '\n'; //add new line for sort to work
        fputs(buf, stream); //add word to stream
      }
    memset(buf,'\0', 34); //reset all values to null so extra chars are removed
    length = 0; //reset length
    }
  }
  if(length >= 5){ //if EOF is reached, add last word to stream
    fputs(buf, stream);
  }

  int stat;
  fclose(stream); //close stream
  close(psfd[0]); //close all pipe ends
  close(psfd[1]);
  close(ssfd[1]);
  close(ssfd[0]);
  if(wait(&stat) == -1){ //wait for children
    fprintf(stderr, "Wait error, status: %d",stat);
    exit(1);
  }
}