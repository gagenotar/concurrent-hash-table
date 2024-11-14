#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include<inttypes.h>
#include <unistd.h>
#include "commands_struct.h"
#include "hash_struct.h"
#include "rw_lock.h"
#include "fileIO.h"
#include "functions.h"

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

void insert(char *key, uint32_t salary /*, hashRecord *table*/)
{
    // Variables for timestamp and messages
    struct timespec currentTime;
    uint64_t timeStamp; // maybe a little bit of an overkill

    // Convert key (Name) to hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash((const uint8_t *)key, strlen(key));

    // Print command with timestamp
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 ", Insert, %s, %d\n", timeStamp, key, salary);

    // Acquire Write Lock
    pthread_rwlock_wrlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 ", Write Lock Acquired\n", timeStamp);

    // Search for existing record with the same hash
    hashRecord *current = root;
    while (current != NULL)
    {
        if (current->hash == hashedKey)
        {
            // If found, update the salary
            current->salary = salary;
            fprintf(output,"Entry Updated\n");

            // Release Write Lock
            pthread_rwlock_unlock(&rw_lock);
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
            fprintf(output,"%" PRIu64 ", Write Lock Released\n", timeStamp);

            return;
        }
        current = current->next;
    }

    // If not found, create a new record
    hashRecord *entry = newRecord(Jenkins_one_at_a_time_hash((const uint8_t *)key, strlen(key)), key, salary);
    // Insert the new record at the beginning of the list
    entry->next = root;
    root = entry;
    fprintf(output,"New Entry Created\n");

    // Release Write Lock
    pthread_rwlock_unlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 ", Write Lock Released\n", timeStamp);
}

void delete_entry(const char *key, hashRecord **table)
{
    if (table == NULL)
    {
        printf("Table is empty or not initialized.\n");
        return;
    }

    // Convert key (Name) to hash
    uint32_t hashedKey = Jenkins_one_at_a_time_hash((const uint8_t *)key, strlen(key));

    // Acquire Lock
    pthread_rwlock_wrlock(&rw_lock);
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    uint64_t timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 " Write Lock Acquired\n", timeStamp);

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
            fprintf(output,"Record with name '%s' and hash %u deleted.\n", key, hashedKey);
            free(current);

            // Release Lock
            pthread_rwlock_unlock(&rw_lock);
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
            fprintf(output,"%" PRIu64 ", Write Lock Released\n", timeStamp);

            return;
        }
        prev = current;
        current = current->next;
    }

    // Record not found
    fprintf(output,"Record with name '%s' not found.\n", key);

    // Release Lock
    pthread_rwlock_unlock(&rw_lock);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    timeStamp = (uint64_t)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    fprintf(output,"%" PRIu64 ", Write Lock Released\n", timeStamp);
}

void display_list(hashRecord *root)
{
    hashRecord *current = root;
    while (current != NULL)
    {
        fprintf(output,"Name: %s, Salary: %d\n", current->name, current->salary);
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
        //printf("Insert done: %d/%d\n", inserts_done, numInserts); // Debugging statement
        if (inserts_done == numInserts)
        {
            pthread_cond_broadcast(&cv);
            //printf("All inserts done, broadcast signal sent.\n"); // Debugging statement
        }
        pthread_mutex_unlock(&cv_mutex);
    }
    else if (strcmp(command->action, "delete") == 0)
    {
        // Wait until all insert operations are done
        pthread_mutex_lock(&cv_mutex);
        while (inserts_done < numInserts)
        {
            //printf("Waiting for inserts to complete. Current: %d/%d\n", inserts_done, numInserts); // Debugging statement
            pthread_cond_wait(&cv, &cv_mutex);
        }
        pthread_mutex_unlock(&cv_mutex);

        delete_entry(command->name, &root);
    }
    else if (strcmp(command->action, "search") == 0)
    {
        hashRecord *result = search(command->name);
        if (result != NULL)
        {
            fprintf(output,"Found: Name: %s, Salary: %d\n", result->name, result->salary);
        }
    }
    return NULL;
}