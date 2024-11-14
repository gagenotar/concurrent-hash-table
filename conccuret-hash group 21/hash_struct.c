#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include<inttypes.h>
#include "hash_struct.h"
#include "rw_lock.h"
#include "fileIO.h"
#include "functions.h"



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
    uint32_t hashedKey = Jenkins_one_at_a_time_hash((const uint8_t *)key, strlen(key));

    // Acquire Lock
    pthread_rwlock_rdlock(&rw_lock);
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    uint64_t timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 ", Read Lock Acquired\n", timeStamp);

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
    timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 ", Read Lock Released\n", timeStamp);

    // Return Data
    if (record == NULL)
    {
        fprintf(output,"%s", "No Record Found\n");
        return NULL;
    }
    else
        return record;
}
