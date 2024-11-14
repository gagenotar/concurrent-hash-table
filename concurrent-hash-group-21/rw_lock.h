#ifndef RW_LOCK_H
#define RW_LOCK_H

#include<pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

extern pthread_rwlock_t rw_lock; // Read-write lock
extern pthread_cond_t cv; // Initialize the conditional variable
extern pthread_mutex_t cv_mutex; // Initialize the mutex

#endif