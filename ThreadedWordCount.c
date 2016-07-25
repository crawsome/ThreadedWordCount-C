/* This program reads in a text file,
divides it into 16 segments, and uses
a multithreaded approach to counting
the words

Author: Colin Burke
*/

#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define NUMBER_OF_THREADS 16

typedef struct read_data{
int thisCount;
int blockSize;
int offsetLow;
} read_data;

//declare the mutex
pthread_mutex_t lock;

//prototype for thread function
void* countWords(void*);

//declare our file
int ourFile;


int main(){

    //our struct array
    read_data data_array[NUMBER_OF_THREADS];

    // declare our file pointer
    if((ourFile = open("Assign3.txt",O_RDONLY))< -1)
        return 1;

    // create 16 threads
    pthread_t threads[NUMBER_OF_THREADS];

    // count bytes, divide by 16 = segment
    int count = lseek(ourFile,0,SEEK_END);

    // report total bytes in file (Debugging)
    // printf(" we found %d bytes in the file.\n",count);

    //fill our structure array up for the thread arguments.
    int i;

    for (i = 0; i < NUMBER_OF_THREADS; i++){
        data_array[i].blockSize = (count / 16) +1;
        data_array[i].offsetLow = data_array[i].blockSize*i;
        data_array[i].thisCount = 0;
        }

    //create 16 threads and have them all count their piece of the file
    for(i = 0; i < NUMBER_OF_THREADS; i++){
        pthread_create(&threads[i], NULL, countWords, (void*)&data_array[i]);
        }

    //count variables
    int finalCount = 0;

    //join all 16 threads
    for(i=0; i < NUMBER_OF_THREADS; i++){
        pthread_join(threads[i], NULL);
        }

    //create our final count
      for(i=0; i < NUMBER_OF_THREADS; i++){
        finalCount = data_array[i].thisCount + finalCount;
        }

    //print our result
    printf("The total words in the file are: %d\n",finalCount);

    //prevents compiler warning.
    exit(0);
}

void *countWords(void* args){

    struct read_data* our_data;
    our_data = (struct read_data*)args;

    //enforce the lock on the mutex
    pthread_mutex_lock(&lock);

    //Show blocksize (Debugging)
    //printf(" The block size is:%d\n",our_data.blockSize);

    //declare our buffer, where text will be read into for processing.
    char buffer[our_data->blockSize];

    //show offset location (Debugging)
    //printf(" The offset starting location is at byte:%d\n",our_data.offsetLow);

    //navigate through the file to our calculated location
    lseek(ourFile,our_data->offsetLow,SEEK_SET);

    //read our file data into our buffer, used for counting
    read(ourFile,buffer,our_data->blockSize);

    //lift the lock on the mutex
    pthread_mutex_unlock(&lock);

    //declare our loop control and word counting ints
    int i = 0;

    // zero out our value before we change it.
    our_data->thisCount=0;

    //count whitespaces in selected text
    for(i = 0; i<our_data->blockSize;i++){
        if(buffer[i] == ' '){
            our_data->thisCount++;
            }}

    //print the qty of words counted
    printf(" Counted words in this thread:%d\n",our_data->thisCount);
    pthread_exit(NULL);
    return 0;
}

