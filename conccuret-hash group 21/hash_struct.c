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
