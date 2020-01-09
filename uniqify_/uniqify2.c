/** NOTE: IF AFTER 11/18 9PM IS NOT ACCEPTED, I HAVE MY OLD CODE THAT I SUBMITTED BEFORE DEADLINE COMMENTED AT THE BOTTOM **/
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

void sort(int psfd1[2], int ssfd1[2], int psfd2[2], int ssfd2[2]){
  pid_t child;
  if ((child = fork()) == -1){//create child process
    fprintf(stderr, "Failed to sort\n");
    exit(1);
  }
  else if(child == 0){ //this is the child
    close(psfd1[1]); //close unneeded pipe ends
    close(ssfd1[0]);
    close(psfd2[1]);
    close(psfd2[0]);
    close(ssfd2[1]);
    close(ssfd2[0]);

    dup2(psfd1[0],0); //connect 0 (stdin) to PS pipe read end
    close(psfd1[0]); //close previous fd

    dup2(ssfd1[1],1); //connect 1 (stdout) to SS pipe write end
    close(ssfd1[1]); //close previous fd

    execlp("sort", "sort", NULL); //execute sort using PATH
    exit(1); //shouldn't get here unless error
  }
}

void suppress(int psfd1[2], int psfd2[2], int ssfd1[2], int ssfd2[2]){
  pid_t child;
  if ((child = fork()) == -1){ //create child process
    fprintf(stderr, "Failed to suppress");
    exit(1);
  }
  else if (child == 0){ //this is the child
    close(psfd1[1]); //close unneeded pipe ends
    close(psfd1[0]);
    close(psfd2[1]);
    close(psfd2[0]);
    close(ssfd1[1]);
    close(ssfd2[1]);

    FILE* stream3 = fdopen(ssfd1[0],"r"); //set stream to read end of SS pipes
    FILE* stream4 = fdopen(ssfd2[0],"r");
    char current[34]; //store current word from one stream
    char other[34]; //word from other stream
    char next[34]; //store next word in current stream
    char next2[34]; //store next word in other stream
    int mult=1; //keep track of multiplicity
    fgets(current, 34, stream3); //get initial strings
    fgets(other, 34, stream4);

    /*Merging is pretty much how you'd merge two sorted arrays*/
    while(isalpha(current[0]) && isalpha(other[0])){ //couldn't accurately test if current/other is NULL,
              //so check if first element is a letter (i.e., while current/other are not NULL)
      if (strcmp(current, other) < 0){ //current is before other
        while(fgets(next, 34, stream3) != NULL){ //keep checking in current stream for mult
          if (strcmp(current, next) == 0){
            mult++; //increase mult if words are the same
          }
          else
            break;
        }
        printf("%-5d%s", mult, current); //print when word is unique
        strcpy(current,next); //store next as current
        memset(next, '\0', 34); //reset all values to null in next
        mult = 1; //reset mult
      }

      else if (strcmp(current, other) > 0){ //other is before current
        while(fgets(next2, 34, stream4) != NULL){ //check in stream for mult
          if (strcmp(other, next2) == 0){
            mult++; //increase mult if words are the same
          }
          else
            break;
        }
        printf("%-5d%s", mult, other); //print when word is unique
        strcpy(other,next2); //store next as current
        memset(next2, '\0', 34); //reset all values to null in next
        mult = 1; //reset mult
      }

      else{ //strcmp(current, other) == 0
        mult++; //increase mult
        /* Check if both streams have more of the same word */
        while(fgets(next, 34, stream3) != NULL){ //check in current stream for mult
          if (strcmp(current, next) == 0){
            mult++; //increase mult if words are the same
          }
          else
            break;
        }        
        while(fgets(next2, 34, stream4) != NULL){
          if (strcmp(other, next2) == 0){
            mult++;
          }
          else
            break;
        }        
        printf("%-5d%s", mult, current);
        strcpy(current,next);
        strcpy(other,next2);
        memset(next, '\0', 34);
        memset(next2, '\0', 34);
        mult = 1;
      }
    }

    if (isalpha(current[0])){ //if stream3 still has more words
      while(fgets(next, 34, stream3)!=NULL){
        if (strcmp(current, next) == 0){
          mult++; //increase mult if words are the same
          continue;
        }
        printf("%-5d%s", mult, current); //print when word is unique
        strcpy(current,next); //store next as current
        memset(next, '\0', 34); //reset all values to null in next
        mult = 1; //reset mult
      }
      printf("%-5d%s", mult, current);
    }

    else if (isalpha(other[0])){ //if stream4 still has more words
      while(fgets(next2, 34, stream4)!=NULL){
        if (strcmp(other, next2) == 0){
          mult++; //increase mult if words are the same
          continue;
        }
        printf("%-5d%s", mult, other); //print when word is unique
        strcpy(other,next2); //store next as current
        memset(next2, '\0', 34); //reset all values to null in next
        mult = 1; //reset mult
      }
      printf("%-5d%s", mult, other);
    }

    else { //if neither stream has more words, there are only two words that need to be printed
      if (strcmp(current,other)<0){
        printf("%-5d%s", mult, current);
        printf("%-5d%s", mult, other);
      }
      else{
        printf("%-5d%s", mult, other);
        printf("%-5d%s", mult, current);
      }
    }

    fclose(stream3);
    fclose(stream4);
    exit(0);
  }
}

int main (int argc, char* argv[]){
  int psfd1[2];
  int psfd2[2];
  int ssfd1[2];
  int ssfd2[2]; //pipe fd for parse-sort & for sort-suppress

  if (pipe(psfd1) == -1){
    fprintf (stderr, "Parse-Sort 1 pipe failure\n");
    exit(1);
  }

  if (pipe(psfd2) == -1){
    fprintf (stderr, "Parse-Sort 2 pipe failure\n");
    exit(1);
  }

  if (pipe(ssfd1) == -1){
    fprintf (stderr, "Sort-Suppress 1 pipe failure\n");
    exit(1);
  }

  if (pipe(ssfd2) == -1){
    fprintf (stderr, "Sort-Suppress 2 pipe failure\n");
    exit(1);
  }

  sort(psfd1, ssfd1, psfd2, ssfd2); //two sort processes
  sort(psfd2, ssfd2, psfd1, ssfd1);

  suppress(psfd1, psfd2, ssfd1, ssfd2);

  /* Parse */
  FILE* stream = fdopen(psfd1[1],"w"); //write to PS1 pipe
  FILE* stream2 = fdopen(psfd2[1],"w"); //write to PS2 pipe
  char buf[34]={'\0'}; //initialize as local array of null values; 32 letters + \n + \0
  int length = 0; //count length of word
  char c;
  int alternate = 0; //to alternate between streams for two sort processes

  while((c = fgetc(stdin)) != EOF){ //get char until end of file
    if (isalpha(c)){ //if c is a letter
      if (length < 32){ //max length is 32
        buf[length++] = tolower(c); //all letters are lowercase
      }
    }
    else{ //non-letter char reached
      if(length >= 5){ //min length is 5
        buf[length++] = '\n'; //add new line for sort to work
        if (alternate == 0){
          fputs(buf, stream); //add word to stream
          alternate = 1; //alternate
        }
        else{
          fputs(buf, stream2); //add word to stream2
          alternate = 0;
        }
      }
      memset(buf,'\0', 34); //reset all values to null so extra chars are removed
      length = 0; //reset length
    }
  }
  if(length >= 5){ //if EOF is reached, add last word to stream
    if (alternate == 0){
      fputs(buf, stream); //add word to stream
    }
    else{
      fputs(buf, stream2); //add word to stream2
    }
  }

  int stat;
  fclose(stream); //close stream
  fclose(stream2);
  close(psfd1[0]); //close all pipe ends
  close(psfd1[1]);
  close(psfd2[0]);
  close(psfd2[1]);
  close(ssfd1[0]);
  close(ssfd1[1]);
  close(ssfd2[0]);
  close(ssfd2[1]);
  if(wait(&stat) == -1){ //wait for children
    fprintf(stderr, "Wait error, status: %d",stat);
    exit(1);
  }
}

// /* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/wait.h>
// #include <ctype.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <string.h>

// void sort(int psfd[2], int ssfd[2]){
//   pid_t child;
//   if ((child = fork()) == -1) //create child process
//     fprintf(stderr, "Failed to sort\n");
//   else if(child == 0){ //this is the child
//     close(psfd[1]); //close unneeded pipe ends
//     close(ssfd[0]);

//     dup2(psfd[0],0); //connect 0 (stdin) to PS pipe read end
//     close(psfd[0]); //close previous fd

//     dup2(ssfd[1],1); //connect 1 (stdout) to SS pipe write end
//     close(ssfd[1]); //close previous fd

//     execlp("sort", "sort", (char*) NULL); //execute sort using PATH
//     exit(1); //shouldn't get here unless error
//   }
// }

// void suppress(int psfd1[2], int psfd2[2], int ssfd1[2], int ssfd2[2]){
//   pid_t child;
//   if ((child = fork()) == -1) //create child process
//     fprintf(stderr, "Failed to suppress");
//   else if (child == 0){ //this is the child
//     close(psfd1[1]); //close unneeded pipe ends
//     close(psfd1[0]);
//     close(psfd2[1]);
//     close(psfd2[0]);
//     close(ssfd1[1]);
//     close(ssfd1[0]);

//     FILE* stream3 = fdopen(ssfd1[0],"r"); //set stream to read end of SS pipes
//     FILE* stream4 = fdopen(ssfd2[0],"r");
//     char current[34]; //store current word from one stream
//     char other[34]; //word from other stream
//     char next[34]; //store next word in current stream
//     char next2[34]; //store next word in other stream
//     int mult=1; //keep track of multiplicity
//     fgets(current, 34, stream3);
//     fgets(other, 34, stream4);
    
//     while(fgets(next, 34, stream3)!=NULL && fgets(next2, 34, stream4)!=NULL){
//       if (strcmp(current, other) < 0){ //current is before other
//         while(fgets(next, 34, stream3) != NULL){ //check in current stream for mult
//           if (strcmp(current, next) == 0){
//             mult++; //increase mult if words are the same
//           }
//           else
//             break;
//         }
//           printf("%-5d%s", mult, current); //print when word is unique
//           strcpy(current,next); //store next as current
//           memset(next, '\0', 34); //reset all values to null in next
//           mult = 1; //reset mult
//       }

//       else if (strcmp(current, other) > 0){ //other is before current
//         while(fgets(next2, 34, stream4) != NULL){ //check in stream for mult
//           if (strcmp(other, next2) == 0){
//             mult++; //increase mult if words are the same
//           }
//           else
//             break;
//         }
//           printf("%-5d%s", mult, other); //print when word is unique
//           strcpy(other,next2); //store next as current
//           memset(next2, '\0', 34); //reset all values to null in next
//           mult = 1; //reset mult
//       }

//       else{ //strcmp(current, other) == 0
//         while(fgets(next, 34, stream3) != NULL){ //check in current stream for mult
//           if (strcmp(current, next) == 0){
//             mult++; //increase mult if words are the same
//           }
//           else
//             break;
//         }
//         while(fgets(next2, 34, stream4) != NULL){
//           if (strcmp(other, next2) == 0){
//             mult++;
//           }
//           else
//             break;
//         }
//         printf("%-5d%s", mult, current);
//         strcpy(current,next);
//         strcpy(other,next2);
//         memset(next, '\0', 34);
//         memset(next2, '\0', 34);
//         mult = 1;
//       }
//     }

//     if (strcmp(next,"") != 0){
//       strcpy(current,next); //store next as current
//       memset(next, '\0', 34); //reset all values to null in next
//       while(fgets(next, 34, stream3)!=NULL){
//         if (strcmp(current, next) == 0){
//           mult++; //increase mult if words are the same
//           continue;
//         }
//         printf("%-5d%s", mult, current); //print when word is unique
//         strcpy(current,next); //store next as current
//         memset(next, '\0', 34); //reset all values to null in next
//         mult = 1; //reset mult
//       }
//       printf("%-5d%s", mult, current);
//     }

//     if (strcmp(next2,"") != 0){
//       strcpy(other,next2); //store next as current
//       memset(next2, '\0', 34); //reset all values to null in next
//       while(fgets(next2, 34, stream4)!=NULL){
//         if (strcmp(other, next2) == 0){
//           mult++; //increase mult if words are the same
//           continue;
//         }
//         printf("%-5d%s", mult, other); //print when word is unique
//         strcpy(other,next2); //store next as current
//         memset(next2, '\0', 34); //reset all values to null in next
//         mult = 1; //reset mult
//       }
//       printf("%-5d%s", mult, other);
//     }

//     if (strcmp(next,"") == 0 && strcmp(next2,"") == 0){
//       if (strcmp(current,other)==0||strcmp(current,other)<0){
//         printf("%-5d%s", mult, current);
//       }
//       else{
//         printf("%-5d%s", mult, other);
//       }
//     }

//     fclose(stream3);
//     fclose(stream4);
//     exit(0);
//   }
// }

// int main (int argc, char* argv[]){
//   int psfd1[2], psfd2[2], ssfd1[2], ssfd2[2]; //pipe fd for parse-sort & for sort-suppress

//   if (pipe(psfd1) == -1){
//     fprintf (stderr, "Parse-Sort 1 pipe failure\n");
//     exit(1);
//   }

//   if (pipe(psfd2) == -1){
//     fprintf (stderr, "Parse-Sort 2 pipe failure\n");
//     exit(1);
//   }

//   if (pipe(ssfd1) == -1){
//     fprintf (stderr, "Sort-Suppress 1 pipe failure\n");
//     exit(1);
//   }

//   if (pipe(ssfd2) == -1){
//     fprintf (stderr, "Sort-Suppress 2 pipe failure\n");
//     exit(1);
//   }

//   sort(psfd1, ssfd1);
//   sort(psfd2, ssfd2);

//   suppress(psfd1, psfd2, ssfd1, ssfd2);

//   /* Parse */
//   FILE* stream = fdopen(psfd1[1],"w"); //write to PS1 pipe
//   FILE* stream2 = fdopen(psfd2[1],"w"); //write to PS2 pipe
//   char buf[34]={'\0'}; //initialize as local array of null values; 32 letters + \n + \0
//   int length = 0; //count length of word
//   char c;
//   int alternate = 0;

//   while((c = fgetc(stdin)) != EOF){ //get char until end of file
//     if (isalpha(c)){ //if c is a letter
//       if (length < 32){ //max length is 32
//         buf[length++] = tolower(c); //all letters are lowercase
//       }
//     }
//     else{ //non-letter char reached
//       if(length >= 5){ //min length is 5
//         buf[length++] = '\n'; //add new line for sort to work
//         if (alternate == 0){
//           fputs(buf, stream); //add word to stream
//           alternate = 1; 
//         }
//         else{
//           fputs(buf, stream2); //add word to stream2
//           alternate = 0;
//         }
//       }
//     }
//     memset(buf,'\0', 34); //reset all values to null so extra chars are removed
//     length = 0; //reset length
//   }
//   if(length >= 5){ //if EOF is reached, add last word to stream
//     if (alternate == 0){
//     fputs(buf, stream); //add word to stream
//     }
//     else{
//     fputs(buf, stream2); //add word to stream2
//     }
//   }

//   int stat;
//   fclose(stream); //close stream
//   close(psfd1[0]); //close all pipe ends
//   close(psfd1[1]);
//   close(psfd2[0]);
//   close(psfd2[1]);
//   close(ssfd1[0]);
//   close(ssfd1[1]);
//   close(ssfd2[0]);
//   close(ssfd2[1]);
//   if(wait(&stat) == -1){ //wait for children
//     fprintf(stderr, "Wait error, status: %d",stat);
//     exit(1);
//   }
// }