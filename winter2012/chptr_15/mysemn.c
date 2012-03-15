#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sem.h>
#include <errno.h>
#include <unistd.h>
#include "mysemn.h"

#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_ROTH | S_IWOTH)
#define KEY ((key_t)1)

sem_t *mysem_open(const char *name, int oflag, mode_t mode, unsigned int value){
	sem_t *wat;
	key_t k;
	int soup, semid;
	if((soup = open(name, O_CREAT | O_EXCL, mode)) > 0){
		if((wat = fdopen(soup, mode)) == -1)
			return NULL;
	}
	if(soup == -1 && errno != EEXIST)
		return NULL;
	if(soup == -1 && errno == EEXIST){
		if((wat = fopen(name, mode)) == -1)
			return NULL;
	}
	if((k = ftok(name, KEY)) == -1)
		return NULL;
	if((semid = semget(k, 1, PERMS | IPC_CREAT)) == -1)
		return NULL;
	if(fprintf(wat, "%d", semid) < 1)
		return NULL;
	return wat;
}
int mysem_close(mysem_t *sem){
	int semid, fd;
	struct stat *buf;
	fd = fileno(sem);
	if(fstat(fd, buf) == -1)
		return -1;
	if(buf.st_nlink == (nlink_t)1){
		rewind(sem);
		while(fread(&semid, sizeof(int), 1, sem) < sizeof(int));
		if(semctl(semid, 0, IPC_RMID) == -1)
			return -1;
	}
	if(fclose(sem) == -1)
		return -1;
	return 0;
}
int mysem_unlink(const char *name){
	mysem_t sem;
	if((sem = fopen(name, "w+")) == -1)
		return -1;
	return mysem_close(sem);
}
int mysem_wait(mysem_t *sem){
	int semid;
	rewind(sem);
	while(fread(&semid, sizeof(int), 1, sem) < sizeof(int));
	struct sembuf myop[1];
	setsembuf(myop, 0, 0, 0);
	if(semop(*sem, myop, 1) == -1)
		return -1;
	return 0;
}
int mysem_post(mysem_t *sem){
	int semid;
	rewind(sem);
	while(fread(&semid, sizeof(int), 1, sem) < sizeof(int));
	struct sembuf myop[1];
	setsembuf(myop, 0, 1, 0);
	if(semop(*sem, myop, 1) == -1)
		return -1;
	return 0;
}

