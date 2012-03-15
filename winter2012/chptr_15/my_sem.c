#include "mysem.h"

int mysem_init(mysem_t *sem, int pshared, unsigned int value){
	if ((*sem = semget(IPC_PRIVATE, 1, PERMS)) == -1)
		return -1;
	return 0;
}

int mysem_destroy(mysem_t *sem){
	if(semctl(*sem, 0, IPC_RMID) == -1)
		return -1;
	return 0;
}

int mysem_wait(mysem_t *sem){
	struct sembuf myop[1];
	setsembuf(myop, 0, 0, 0);
	if(semop(*sem, myop, 1) == -1)
		return -1;
	return 0;
}

int mysem_post(mysem_t *sem){
	struct sembuf myop[1];
	setsembuf(myop, 0, 1, 0);
	if(semop(*sem, myop, 1) == -1)
		return -1;
	return 0;
}
