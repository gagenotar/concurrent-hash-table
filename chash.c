//This is preliminary file add and remove as you see fit

#include <cstdint>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#ifdef linux
#include <semaphor.h>
#endif

typedef struct hash_struct
{
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

int main(int argc, char *argv[])
{
    // variables
    hashRecord* table = NULL; // lead with given test data

    //  tests
    // new adds
    insert('Richard Garriot', 40000, &table);
    insert('Sid Meier', 50000, &table);
    insert('Shigeru Miyamoto', 51000, &table);
    insert('Hideo Kojima', 45000, &table);
    insert('Gabe Newell', 49000, &table);
    insert('Roberta Williams', 45900, &table);
    insert('Carol Shaw', 41000, &table);

    // Updates
    insert('Carol Shaw', 51000, &table);
    insert('Shigeru Miyamoto', 61000, &table);
    insert('Hideo Kojima', 55000, &table);

    // additional tests if we would like :)
}

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

void insert(const char* key, uint32_t salary, hashRecord** table)
{
    // variables
    struct timespec currentTime;
    long long timeStamp; // maybe a little bit of an overkill
    String commandMsg = "";
    String lockMsg = "";
    hashed_key = Jenkins_one_at_a_time_hash(key, len(key)); // idk if it is right

    // Prints
    // command
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%d, Insert, %s, %d", timeStamp, key, salary);
    // locks
    timeStamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%d, Read Lock Acquired ", timeStamp);

    // search
    hashRecord* entry = search(*table, hashed_key); // include search file once ready
    // Present?
    if (entry == NULL)
    { // new Data
        }
    else
    { // update
        // add check if needed
        entry.salary = salary
    }

    // release lock
    // print
    time_stamp = (long long)currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
    printf("%d, Read Lock RELEASED ", timeStamp);
}


hashRecord* search(hashRecord *root, const char* key) {
	//Convert key (Name) to hash
	uint32_t hashedKey = Jenkins_one_at_a_time_hash(key, strlen(key));

    //Acquire Lock
    //Timestamp

	//Linear Search O(n)
	hashRecord *record = NULL;
	while(root != NULL){

		if(root->hash == hashedKey){
			record = root;
			break;
		}

		root = root->next;
	}
	
    //Release Lock
    //Timestamp 
	
	//Return Data
	if(record == NULL){
		printf("%s", "No Record Found\n");
		return NULL;
	}
	else
		return record;
}
