/*
Abdulellah Abualshour | RUID: 158006704 | netid: aha59
Marshal Nink | RUID:158009719 | netid: mrn61
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

void compressT_LOLS(char*, int);
void* helper(void*);

FILE * bigfile; //file to be loaded
char * fn; //filename
int n; //number of parts

int x;
int x2;
int y;

//main function for entering specific arguments for the compression algorithm:
int main() {
    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
    //Here you should modify to what the number of parts you want is:
    compressT_LOLS("./filex.java", 3);
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

//compression algorithm:
void compressT_LOLS(char* filename, int numberOfParts){
    fn = filename;
    n = numberOfParts;
    //open bigfile
    int startIndex; //start index for string manpiulation
    bigfile = fopen(fn, "r");

    //error checking:
    if (bigfile == NULL){
        printf("ERROR: File could not be opened.\n");
        return;
    }
    
    //make sure the file is loaded into the string:
    char * str = 0;
    long strlength;
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
    int length = strlen(str);
    int count = 0;
    while(count < length){
        if(!isalpha(str[count])){
            str[count] = ' ';
        }
        count++;
    }

    //remove spaces:
    RemoveSpaces(str);
    
    if(strlen(str) < numberOfParts){
        printf("ERROR: Length of string is less than number of parts intended to be created.\n");
        return;
    }

    //see the number of characters mod number of parts to see if we have odd or even 
    strlength = strlen(str);
    x = strlength%numberOfParts;
    x2 = x;
    y = strlength/numberOfParts;    
    startIndex = 0;

    // spawn the number of threads needed 
    int i;
    pthread_t * tid = malloc(sizeof(pthread_t)*numberOfParts);
    int table[numberOfParts][4];

    int m = 0;
    int w = 0;

    //table to be passed to thread - start indeces:
    for(w = 0; w < numberOfParts; w++){
        if(x != 0){
            table[w][m] = startIndex;
            x--;
            startIndex = startIndex+y+1;
        }
        else{
            table[w][m] = startIndex;
            startIndex = startIndex+y;
        }
    }

    m++;
    //table to be passed to thread - thread numbers:
    for(w = 0; w < numberOfParts; w++){
        table[w][m] = w;
    }

    m++;
    //table to be passed to thread - remainder flag
    for(w = 0; w < numberOfParts; w++){
        if(x2 != 0){
            table[w][m] = 1;
            x2--;
        }
        else{
            table[w][m] = 0;
        }
    }

    //spawn threads + pass table:
    for (i = 0; i < numberOfParts; i++) {
        pthread_create(&tid[i], NULL, &helper, &table[i]);
    }
    
    //Joining all threads:
    for (i = 0; i < numberOfParts; i++) {
        pthread_join(tid[i], NULL);
    }
}
/*
    TABLE:
        The passed table row to the helper will contain the following:

        index       content
        0 --------- starting index
        1 --------- thread number
        2 --------- remainder flag
*/
//helper method the argument passed is a table row that provides us with metadata about where to start and how to compress the text in the file:
void* helper(void* ind){
    //extract table from pointer:
    int * table = (int*)ind;
    int size; //for string size

    // open file
    FILE* fp = fopen(fn, "r");
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

    //remove whitespace and unnecessary characters:=
    int count = 0;
    while(count < size){
        if(!isalpha(str[count])){
            str[count] = ' ';
        }
        count++;
    }

    //remove spaces:
    RemoveSpaces(str);
    char streamStack1[y+2];
    char streamStack2[y+1];

    //pthread_mutex_lock(&helperMutex);
    if(table[2] != 0){
        streamStack1[y+1] = '\0';
        memcpy(streamStack1, &str[table[0]], y+1);

        //Begin encoding:
        printf("(**)Encoding in thread #%d:\t %s\n",table[1], streamStack1);
        char encoded[strlen(streamStack1)];
        int b=0;
        for(b=0; b < strlen(encoded); b++){
            encoded[b] = '\0';
        }
        int i=0,k;
        
        //comparing the characters and manipulating the original string to contain the compressed version:
        for(i=0; streamStack1[i]; i++){
            for(k=1; streamStack1[i] == streamStack1[i+1]; k++, i++);
            if(i == 0){ //only one character at the begginning
                sprintf(&encoded[0],"%c",streamStack1[i]);
            }
            else if(k == 1){ //only one occurrence, do not compress:
                sprintf(&encoded[strlen(encoded)],"%c", streamStack1[i]);
            }
            else if(k == 2){ //only two occurrence, do not compress:
                sprintf(&encoded[strlen(encoded)],"%c%c",streamStack1[i], streamStack1[i]);
            }
            else{ //more than two:
                sprintf(&encoded[strlen(encoded)],"%d%c",k,streamStack1[i]);
            }
        }

        //print results:
        printf("(**)Encoded in thread #%d:\t %s\n",table[1], encoded);  

        //output file creation:         
        FILE* op;
        //snum has the file number:
        char snum[5];
        sprintf(snum, "%d", table[1]);

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
        //printf("2:%s\n", fnCompressed);
        //concat the file number to the file name:
        char name[6] = "_LOLS";
        char* encodedX = (char*)malloc(strlen(encoded)+1); //to prevent data corruption
        memcpy(encodedX, &encoded[0], strlen(encoded)+1);
        strcat(fnCompressed, name);
        if(n != 1){
            strcat(fnCompressed, snum);
        }
                //successfully created the file:
        op = fopen(fnCompressed, "w");
        if (op == NULL){
            printf("ERROR: File could not be created.\n");
            return NULL;
        }

        //printf("final: %s\n", encodedX);
        //copy data to file:
        fputs(encodedX, op);

        //close file:
        fclose(op);
        printf("**** Successfully Compressed and Created %s ****\n", fnCompressed);
        x2--;
    }

    //If we have even characters in this current thread to spawn:
    else{
        streamStack2[y] = '\0';
        memcpy(streamStack2, &str[table[0]], y);

        //Begin encoding:
        printf("(**)Encoding in thread #%d:\t %s\n",table[1], streamStack2);
        char encoded[strlen(streamStack2)];
        int b=0;
        for(b=0; b < strlen(encoded); b++){
            encoded[b] = '\0';
        }
        int i=0,k;
        
        //comparing the characters and manipulating the original string to contain the compressed version:
        for(i=0; streamStack2[i]; i++){
            for(k=1; streamStack2[i] == streamStack2[i+1]; k++, i++);
            if(i == 0){ //only one character at the begginning
                sprintf(&encoded[0],"%c",streamStack2[i]);
            }
            else if(k == 1){ //only one occurrence, do not compress:
                sprintf(&encoded[strlen(encoded)],"%c", streamStack2[i]);
            }
            else if(k == 2){ //only two occurrence, do not compress:
                sprintf(&encoded[strlen(encoded)],"%c%c",streamStack2[i], streamStack2[i]);
            }
            else{ //more than two:
                sprintf(&encoded[strlen(encoded)],"%d%c",k,streamStack2[i]);
            }
        }

        //print results:
        printf("(**)Encoded in thread #%d:\t %s\n",table[1], encoded);  

        //output file creation:         
        FILE* op;

        //snum has the file number:
        char snum[5];
        sprintf(snum, "%d", table[1]);

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
        //fnCompressed[strlen(fn)] = '\0';
        //concat the file number to the file name:
        char name[18] = "_LOLS";
        char* encodedX = (char*)malloc(strlen(encoded)+1); //to prevent data corruption
        memcpy(encodedX, &encoded[0], strlen(encoded)+1);
        strcat(fnCompressed, name);
        if(n != 1){
            strcat(fnCompressed, snum);
        }
        
        //successfully created the file:
        op = fopen(fnCompressed, "w");
        if (op == NULL){
            printf("ERROR: File could not be created.\n");
            return NULL;
        }

        //printf("final: %s\n", encodedX);
        //copy data to file:
        fputs(encodedX, op);

        //close file:
        fclose(op);
        printf("**** Successfully Compressed and Created %s ****\n", fnCompressed);
    }    
    return NULL;
}