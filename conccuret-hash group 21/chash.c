#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "commands_struct.h"
#include "hash_struct.h"
#include "rw_lock.h"
#include "functions.h"
#include "fileIO.h"


FILE* input = NULL;
FILE* output = NULL;
pthread_rwlock_t rw_lock; // Read-write lock
pthread_cond_t cv = PTHREAD_COND_INITIALIZER; // Initialize the conditional variable
pthread_mutex_t cv_mutex = PTHREAD_MUTEX_INITIALIZER; // Initialize the mutex
hashRecord *root = NULL; // Head of the linked list
int inserts_done = 0; // Number of insert operations completed
int numInserts = 0; // Number of insert commands

int main(int argc, char *argv[])
{
    input = fopen("commands.txt","r");


    // Create the input and output files
    // Make sure that your commands.txt input file is in the same
    // directory as the .c file so it can read it.
    //FILE* input = fopen("commands.txt","r");
    //FILE* output = fopen("output.txt","w");   

    // Create a buffer to read each line 
    char line[256];

    // Get the next line and tokenize it
    fgets(line,sizeof(line),input);
    char* token = strtok(line,",");
    token = strtok(NULL,",");

    // Get the number of threads 
    int numThreads = atoi(token);

    // Read the last two tokens on the line
    token = strtok(NULL,",");
    token = strtok(NULL,",");

    // Create an array to store all of the commands 
    struct Commands commandsArray[numThreads];
    
    // Read the remaining lines of input and store each value into an array 
    for(int i=0;i<numThreads;i++)
    {
        // Get the next line of input
        fgets(line,sizeof(line), input);

        // Partition that line into tokens of action, name, salary
        token = strtok(line,",");

        // Get the values of all of the struct 
        strcpy(commandsArray[i].action, token);
        token = strtok(NULL,",");

        strcpy(commandsArray[i].name, token);
        token = strtok(NULL,",");

        commandsArray[i].salary = atoi(token);
        token = strtok(NULL,",");

        // Check if the action is an insert command
        if (strcmp(commandsArray[i].action, "insert") == 0)
        {
            numInserts++;
        }
    }

    // Close the input file 
    fclose(input);
    output = fopen("output.txt","w");

    pthread_rwlock_init(&rw_lock, NULL); // Initialize the read-write lock

    // Create and execute threads
    pthread_t threads[numThreads];
    for (int i = 0; i < numThreads; i++)
    {
        pthread_create(&threads[i], NULL, execute_command, (void *)&commandsArray[i]);
    }

    // Join threads
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Destroy the read-write lock
    pthread_rwlock_destroy(&rw_lock);

    // Display the final state of the hash table
    display_list(root);

    //close the file
    fclose(output);
}
