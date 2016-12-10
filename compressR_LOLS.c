/*
Abdulellah Abualshour | RUID: 158006704 | netid: aha59
Marshal Nink | RUID:158009719 | netid: mrn61
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>

//function declarations:
void compressR_LOLS(char* filename, int);
int processNum = 0;

FILE *bigfile; //file to be loaded
int startIndex; //start index for string manpiulation
char * fn; //filename
int n; //number of parts

//main function for entering specific arguments for the compression algorithm (our own driver):
int main() {
    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
    //Here you should modify to what the number of parts you want is:
    compressR_LOLS("./newfile.c", 1);
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed time: %fms.\n", elapsedTime);
    return 0;
}

//function for removing whitespace:
void RemoveSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}

//fetching:
void compressR_LOLS(char* filename, int numberOfParts) {
    fn = filename;
    n = numberOfParts;
    //open bigfile
    bigfile = fopen(fn, "r");

    //error checking:
    if (bigfile == NULL){
        printf("ERROR: File could not be opened.\n");
        return;
    }
    
    //make sure the file is loaded into the string:
    long strlength = 0;
    int processNum = 0;
    char * str = 0;
    if (bigfile)
    {
      fseek (bigfile, 0, SEEK_END);
      strlength = ftell (bigfile);
      fseek (bigfile, 0, SEEK_SET);
      str = malloc (strlength);
      if (str)
      {
        fread (str, 1, strlength, bigfile);
      }
      fclose (bigfile);
    }

    //error checking:
    if(strlength == 0){
        printf("ERROR: File has no content.\n");
        return;
    }
    printf("============================\n(**)Original Input from file:\n%s\n============================\n",str);

    //remove whitespace and unnecessary characters:
    int lengthx = strlen(str);
    int count = 0;
    while(count < lengthx){
        if(!isalpha(str[count])){
            str[count] = ' ';
        }
        count++;
    }

    //remove spaces:
    RemoveSpaces(str);
    
    //set the new length(after clean):
    strlength = strlen(str);

    //error check:
    if(strlength < numberOfParts){
        printf("ERROR: Length of string is less than number of parts intended to be created.\n");
        return;
    }

    //see the number of characters mod number of parts to see if we have odd or even 
    int x = strlength%numberOfParts;
    int y = strlength/numberOfParts;    
    startIndex = 0;
    
    // spawn the number of processes needed 
    pid_t pid;
    int i;
    char length[10], start_index[10], process_num[10], np[10];

    /*
        args[0] is terminal command
        args[1] is file name / path
        args[2] is length of the text to be compressed
        args[3] is the starting index of the compression in the file
        args[4] is process number
    */
    char* args[] = {"./compressR_worker_LOLS", fn, length, start_index, process_num, np, NULL};          
    for(i = 0; i < numberOfParts; i++){
        /* Setup for the Child processes */

        // first one, send y+1
        if(x!=0) {
            sprintf(length, "%d", y+1);
            sprintf(start_index, "%d", startIndex);
            sprintf(process_num, "%d", processNum);
            sprintf(np, "%d", n);
            x--;
            startIndex += (y+1);
            pid = fork();
        } 
        // rest, just send y
        else {
            sprintf(length, "%d", y);
            sprintf(start_index, "%d", startIndex);
            sprintf(process_num, "%d", processNum);
            sprintf(np, "%d", n);
            startIndex += y;
            pid = fork();
        }

        /*  Create and run the child processes as workers  */
        if(pid < 0){
            printf("ERROR: fork failed, could not spawn process.\n");
            return;
        }
        else if(pid == 0){//child
            //exec here
            //printf("\nargs[1]:\t%s\nargs[2]:\t%s\nargs[3]:\t%s\nargs[4]:\t%s\n \n",args[1],args[2],args[3],args[4]);
            execv(args[0], args);
        }
        processNum++;
    }

    //wait on all children before termination:
    int o;
    for(o = 0; o < processNum; o++){
        wait(NULL);
    }
    return;
}