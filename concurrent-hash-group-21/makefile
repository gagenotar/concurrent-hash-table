chash:chash.o functions.o hash_struct.o
	gcc -o chash chash.o functions.o hash_struct.o -lpthread

functions.o: functions.c hash_struct.h rw_lock.h commands_struct.h functions.h fileIO.h
	gcc -std=gnu99 -Wall -c functions.c 

hash_struct.o: hash_struct.c hash_struct.h rw_lock.h fileIO.h functions.h
	gcc -std=gnu99 -Wall -c hash_struct.c 

chash.o: chash.c hash_struct.h rw_lock.h commands_struct.h functions.h fileIO.h
	gcc -std=gnu99 -Wall -c chash.c 



