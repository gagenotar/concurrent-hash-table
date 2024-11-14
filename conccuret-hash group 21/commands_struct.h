#ifndef COMMANDS_STRUCT_H
#define  COMMANDS_STRUCT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct Commands
{
    char action[10];
    char name[50];
    int salary;
};

#endif