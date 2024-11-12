#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <filesystem>

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

hashRecord *root = NULL;
pthread_rwlock_t rw_lock;
pthread_condition_t id_lock;

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
    fprintf(FILE_NAME, "Searching for %s\n", key);

    // Compute hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    // Acquire Lock
    pthread_rwlock_rdlock(&rw_lock);
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    long long timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Read Lock Acquired\n", timeStamp);

    // Convert key (Name) to hash
    // uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

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
    pthread_conditional_lock(&id_lock);
    // variables
    struct timespec currentTime;
    long long timeStamp; // maybe a little bit of an overkill
    char *commandMsg = "";
    char *lockMsg = "";

    // Convert key (Name) to hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    // Prints command
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Insert, %s, %d\n", timeStamp, key, salary);

    // Acquire Lock
    pthread_rwlock_wrlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Write Lock Acquired\n", timeStamp);

    // search
    hashRecord *current = root;
    while (current != NULL)
    {
        if (current->hash == hashedKey)
        {
            current->salary = salary;
            printf("Entry Updated\n");

            // Release Lock
            pthread_rwlock_unlock(&rw_lock);
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
            printf("%lld, Write Lock Released\n", timeStamp);

            return;
        }
        current = current->next;
    }

    // create new entry
    hashRecord *entry = newRecord(Jenkins_one_at_a_time_hash(key, strlen(key)), key, salary);
    // insert
    entry->next = root;
    root = entry;
    printf("New Entry Created\n");

    // Release Lock
    pthread_rwlock_unlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Write Lock Released\n", timeStamp);

    // print
    // time_stamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    // printf("%d, Read Lock RELEASED ", timeStamp);
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

int main(int argc, char *argv[])
{
    /*          *******for the implementation the comment sections can change based on I/O********               */

    // var
    pthread_rwlock_a(&rw_lock, NULL);
    pthread_conditional_t(&id_lock, NULL); // conditional variable for inserts and deletes
    pthread_t *threads;                    // = (pthread_T*)malloc(sizeof(pthread_t) * num_threads) add section after I/O
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
    pthread_t *threads; //= (pthread_t*)malloc(num_threads * sizeof(pthread_t)); //threads
    pthread_attr_t threads_attributes;
    // to access just do threads[i] and to assign task do pthread_create(thread[i], attr, function, arg) [attr == null]
}
