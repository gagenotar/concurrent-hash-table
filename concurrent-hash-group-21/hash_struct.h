#ifndef HASH_STRUCT_H
#define HASH_STRUCT_H

#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct hash_struct
{
    uint32_t hash;
    char *name;
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

extern hashRecord *root; // Head of the linked list

hashRecord *newRecord(uint32_t h, char *n, uint32_t s);

#endif