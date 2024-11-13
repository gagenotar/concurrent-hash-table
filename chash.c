#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
// #include <filesystem.h>

// #ifdef linux
// #include <semaphor.h>
// #endif
FILE *fp;
typedef struct hash_struct
{
    uint32_t hash;
    char *name;
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

// Struct to track each of the commands  
struct Commands
{
    char action[10];
    char name[50];
    int salary;
};

hashRecord *root = NULL;
pthread_rwlock_t rw_lock;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER; // Initialize the conditional variable
pthread_mutex_t cv_mutex = PTHREAD_MUTEX_INITIALIZER; // Initialize the mutex
int inserts_done = 0; // Number of insert operations completed
int numInserts = 0; // Number of insert commands

uint32_t Jenkins_one_at_a_time_hash(const uint8_t *key, size_t length)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

hashRecord *newRecord(uint32_t h, char *n, uint32_t s)
{
    hashRecord *nR = malloc(sizeof(hashRecord));
    nR->hash = h;
    nR->name = n;
    nR->salary = s;
    nR->next = NULL;

    return nR;
}

hashRecord *search(/*hashRecord *root,*/ char *key)
{
    // fprintf(FILE_NAME, "Searching for %s\n", key);

    // Compute hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    // Acquire Lock
    pthread_rwlock_rdlock(&rw_lock);
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    long long timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Read Lock Acquired\n", timeStamp);

    // Linear Search O(n)
    hashRecord *record = NULL;
    hashRecord *current = root;
    while (current != NULL)
    {
        if (current->hash == hashedKey)
        {
            record = current;
            break;
        }
        current = current->next;
    }

    // Release Lock
    pthread_rwlock_unlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Read Lock Released\n", timeStamp);

    // Return Data
    if (record == NULL)
    {
        printf("%s", "No Record Found\n");
        return NULL;
    }
    else
        return record;
}

void insert(char *key, uint32_t salary /*, hashRecord *table*/)
{
    // Variables for timestamp and messages
    struct timespec currentTime;
    long long timeStamp; // maybe a little bit of an overkill
    char *commandMsg = "";
    char *lockMsg = "";

    // Convert key (Name) to hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    // Print command with timestamp
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Insert, %s, %d\n", timeStamp, key, salary);

    // Acquire Write Lock
    pthread_rwlock_wrlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Write Lock Acquired\n", timeStamp);

    // Search for existing record with the same hash
    hashRecord *current = root;
    while (current != NULL)
    {
        if (current->hash == hashedKey)
        {
            // If found, update the salary
            current->salary = salary;
            printf("Entry Updated\n");

            // Release Write Lock
            pthread_rwlock_unlock(&rw_lock);
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
            printf("%lld, Write Lock Released\n", timeStamp);

            return;
        }
        current = current->next;
    }

    // If not found, create a new record
    hashRecord *entry = newRecord(Jenkins_one_at_a_time_hash(key, strlen(key)), key, salary);
    // Insert the new record at the beginning of the list
    entry->next = root;
    root = entry;
    printf("New Entry Created\n");

    // Release Write Lock
    pthread_rwlock_unlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Write Lock Released\n", timeStamp);
}

void delete(const char *key, hashRecord **table)
{
    if (table == NULL)
    {
        printf("Table is empty or not initialized.\n");
        return;
    }

    // Convert key (Name) to hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    // Acquire Lock
    pthread_rwlock_wrlock(&rw_lock);
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    long long timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Write Lock Acquired\n", timeStamp);

    // Traverse the list to find the record
    hashRecord *current = root;
    hashRecord *prev = NULL;

    while (current != NULL)
    {
        if (current->hash == hashedKey)
        {
            // If record is found
            if (prev == NULL)
            {
                // The record to delete is the head of the list
                root = current->next;
            }
            else
            {
                // The record to delete is not the head
                prev->next = current->next;
            }
            // Print deletion message and free memory
            printf("Record with name '%s' and hash %u deleted.\n", key, hashedKey);
            free(current);

            // Release Lock
            pthread_rwlock_unlock(&rw_lock);
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
            printf("%lld, Write Lock Released\n", timeStamp);

            return;
        }
        prev = current;
        current = current->next;
    }

    // Record not found
    printf("Record with name '%s' not found.\n", key);

    // Release Lock
    pthread_rwlock_unlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Write Lock Released\n", timeStamp);
}

void display_list(hashRecord *root)
{
    hashRecord *current = root;
    while (current != NULL)
    {
        printf("Name: %s, Salary: %d\n", current->name, current->salary);
        current = current->next;
    }
}

void *execute_command(void *arg)
{
    struct Commands *command = (struct Commands *)arg;

    if (strcmp(command->action, "insert") == 0)
    {
        insert(command->name, command->salary);

        // Signal that an insert operation is done
        pthread_mutex_lock(&cv_mutex);
        inserts_done++;
        printf("Insert done: %d/%d\n", inserts_done, numInserts); // Debugging statement
        if (inserts_done == numInserts)
        {
            pthread_cond_broadcast(&cv);
            printf("All inserts done, broadcast signal sent.\n"); // Debugging statement
        }
        pthread_mutex_unlock(&cv_mutex);
    }
    else if (strcmp(command->action, "delete") == 0)
    {
        // Wait until all insert operations are done
        pthread_mutex_lock(&cv_mutex);
        while (inserts_done < numInserts)
        {
            printf("Waiting for inserts to complete. Current: %d/%d\n", inserts_done, numInserts); // Debugging statement
            pthread_cond_wait(&cv, &cv_mutex);
        }
        pthread_mutex_unlock(&cv_mutex);

        delete(command->name, &root);
    }
    else if (strcmp(command->action, "search") == 0)
    {
        hashRecord *result = search(command->name);
        if (result != NULL)
        {
            printf("Found: Name: %s, Salary: %d\n", result->name, result->salary);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{


    // Create the input and output files
    // Make sure that your commands.txt input file is in the same
    // directory as the .c file so it can read it.
    FILE* input = fopen("commands.txt","r");
    FILE* output = fopen("output.txt","w");   

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

    // var
    // pthread_rwlock_a(&rw_lock, NULL);
    // pthread_conditional_t(&id_lock, NULL); // conditional variable for inserts and deletes
    // pthread_t *threads;                    // = (pthread_T*)malloc(sizeof(pthread_t) * num_threads) add section after I/O
    // may need thread attributes use this code for it place where applicable
    /*
        //put this at top of file
        typdef struct {
            int thread_id;
            //other attributes
        }thread_data_t;

        //leave in this section
        thread_data_t *threads_data =(thread_data_t*)malloc(num_threads*sizeof(thread_data_t));
    */
    // we need a way to run all inserts first idk how? I do have the cv set up for it.
    // pthread_t *threads; //= (pthread_t*)malloc(num_threads * sizeof(pthread_t)); //threads
    // pthread_attr_t threads_attributes;
    // to access just do threads[i] and to assign task do pthread_create(thread[i], attr, function, arg) [attr == null]
}
