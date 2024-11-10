#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// #ifdef linux
// #include <semaphor.h>
// #endif

typedef struct hash_struct
{
    uint32_t hash;
    char *name;
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

// global variable idk if this is good with yall
hashRecord *root = NULL;
pthread_rwlock_t rw_lock;

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
    printf("Searching for %s\n", key);

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
    while (current != NULL) {
        if (current->hash == hashedKey) {
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

void delete(const char* key, hashRecord** table) {
    if (table == NULL) {
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

    while (current != NULL) {
        if (current->hash == hashedKey) {
            // If record is found
            if (prev == NULL) {
                // The record to delete is the head of the list
                root = current->next;
            } else {
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
    pthread_rwlock_init(&rw_lock, NULL);

    // variables
    // lead with given test data
    //  tests
    // new adds
    insert("Richard Garriot", 40000);
    insert("Sid Meier", 50000);
    insert("Shigeru Miyamoto", 51000);
    insert("Hideo Kojima", 45000);
    insert("Gabe Newell", 49000);
    insert("Roberta Williams", 45900);
    insert("Carol Shaw", 41000);

    // Updates
    insert("Carol Shaw", 51000);
    insert("Shigeru Miyamoto", 61000);
    insert("Hideo Kojima", 55000);

    // Search
    search("Carol Shaw");

    display_list(root);

    // additional tests if we would like :)
}
