/*
Abdulellah Abualshour | RUID: 158006704 | netid: aha59
Marshal Nink | RUID:158009719 | netid: mrn61
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

char * fn; //filename

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

//compression algorithm:
void compressR_worker_LOLS(char* file, char* le, char* si, char* processNum, char* n){
    fn = file;
    //values to use later:
    int length = atoi(le);
    int startIndex = atoi(si);
    int size;

    // open file, save contents in string for future use:
    FILE* fp = fopen(file, "r");
    char * str = 0;
    if (fp)
    {
      fseek (fp, 0, SEEK_END);
      size = ftell (fp);
      fseek (fp, 0, SEEK_SET);
      str = malloc (size);
      if (str)
      {
        fread (str, 1, size, fp);
      }
      fclose (fp);
    }
    str[size] = '\0';

    //remove whitespace and unnecessary characters:
    int count = 0;
    while(count < size){
        if(!isalpha(str[count])){
            str[count] = ' ';
        }
        count++;
    }

    //remove spaces:
    RemoveSpaces(str);
    char streamStack[length+1];
    streamStack[length] = '\0';
    memcpy(streamStack, &str[startIndex], length);

    //Begin encoding:
    printf("(**)Encoding in process #%s:\t %s\n",processNum,streamStack);
    char encoded[strlen(streamStack)];
    int b=0;
    for(b=0; b < strlen(encoded); b++){
        encoded[b] = '\0';
    }
    int i,k;
    
    //comparing the characters and manipulating the original string to contain the compressed version:
    for(i=0; streamStack[i]; i++){
        for(k=1; streamStack[i] == streamStack[i+1]; k++, i++);
        if(i == 0){ //only one character at the begginning
            sprintf(&encoded[0],"%c",streamStack[i]);
        }
        else if(k == 1){ //only one occurrence, do not compress:
            sprintf(&encoded[strlen(encoded)],"%c", streamStack[i]);
        }
        else if(k == 2){ //only two occurrence, do not compress:
            sprintf(&encoded[strlen(encoded)],"%c%c",streamStack[i], streamStack[i]);
        }
        else{ //more than two:
            sprintf(&encoded[strlen(encoded)],"%d%c",k,streamStack[i]);
        }
    }

    //print results:
    printf("(**)Encoded in process #%s:\t %s\n",processNum,encoded);  

    //output file creation:         
    FILE* op;

    //extract new file name:
        //fn = "./text.txt";
        char fnCompressed[strlen(fn)];
        fnCompressed[strlen(fn)] = '\0';
        memcpy(fnCompressed, &fn[0], strlen(fn));
        //printf("1:%s\n", fnCompressed);
        int u = 0;
        for(u = 0; u < strlen(fn); u++){
            if((fn[u] == '.') && (u != 0)){
                fnCompressed[u] = '_';
            }
            else if((fn[u] == '.') && (u == 0)){
                fnCompressed[u] = ' ';
            }
            else if(fn[u] == '/'){
                fnCompressed[u] = ' ';
            }
        }
        RemoveSpaces(fnCompressed);
        fnCompressed[strlen(fn)] = '\0';
        //printf("fnCompressed: %s\n", fnCompressed);
        //printf("2:%s\n", fnCompressed);
        //concat the file number to the file name:
        char name[6] = "_LOLS";
        char* encodedX = (char*)malloc(strlen(encoded)+1); //to prevent data corruption
        memcpy(encodedX, &encoded[0], strlen(encoded)+1);
        strcat(fnCompressed, name);
        //printf("fnCompressed: %s\n", fnCompressed);
        if(strcmp(n, "1") != 0){
            strcat(fnCompressed, processNum);
        }
        //printf("fnCompressed: %s\n", fnCompressed);
        //successfully created the file:
        op = fopen(fnCompressed, "w");
        if (op == NULL){
            printf("ERROR: File could not be created.\n");
            return;
        }

        //printf("final: %s\n", encodedX);
        //copy data to file:
        fputs(encodedX, op);

    //close file:
    fclose(op);
    printf("**** Successfully Compressed and Created %s ****\n", fnCompressed);
    return;
}

int main(int argc, char *argv[]){
    //printf("\nargv[1]:\t%s\nargv[2]:\t%s\nargv[3]:\t%s\nargv[4]:\t%s\n \n",argv[1],argv[2],argv[3],argv[4]);
    compressR_worker_LOLS(argv[1], argv[2], argv[3], argv[4], argv[5]); //arguments
    
    return 0;
}

