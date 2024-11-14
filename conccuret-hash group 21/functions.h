#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <stddef.h>
#include "hash_struct.h"

extern int inserts_done;
extern int numInserts;

uint32_t Jenkins_one_at_a_time_hash(const uint8_t *key, size_t length);
void search(/*hashRecord *root,*/ char *key);
void insert(char *key, uint32_t salary);
void delete_entry(const char *key, hashRecord **table);
void display_list(hashRecord *root);
void *execute_command(void *arg);

#endif