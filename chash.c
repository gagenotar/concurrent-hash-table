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

    // Convert key (Name) to hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    // Acquire Lock
    // Timestamp

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
    // Timestamp

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

    // Get current time?
    // clock_gettime(CLOCK_REALTIME, &currentTime);

    // Prints
    // command
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%lld, Insert, %s, %d\n", timeStamp, key, salary);
    // locks
    // timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    // printf("%d, Read Lock Acquired ", timeStamp);

    // search
    hashRecord *entry = search(key); // include search file once ready
    // Present?
    if (entry == NULL)
    { // new Data
        // create new entry
        entry = newRecord(Jenkins_one_at_a_time_hash(key, strlen(key)), key, salary);
        // insert
        entry->next = root;
        root = entry;
        printf("New Entry Created\n");
    }
    else
    { // update
        entry->salary = salary;
        printf("Entry Updated\n");
    }

    // release lock
    // print
    // time_stamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    // printf("%d, Read Lock RELEASED ", timeStamp);
}

void delete()
{
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

    display_list(root);

    // additional tests if we would like :)
}