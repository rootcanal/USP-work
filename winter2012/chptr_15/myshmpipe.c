
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define BUFSIZE 1024

pipe_t *pipe_open(void){
	pipe_t *p;
	int shmid, semid;
	if((shmid = shmget(IPC_PRIVATE, BUFSIZE, S_IRUSR | S_IWUSR)) == -1)
		return NULL;
	if((p = (pipe_t *)shmat(shmid, NULL, 0)) == -1)
		return -1;
	p->shmid = shmid;
	p->data_size = 0;
	p->end_of_file = 0;
	if((semid = semget(IPC_PRIVATE, 2, S_IRUSR | S_IWUSR)) == -1)
		return -1;
	if(initelement(semid, 0, 1) == -1 || initelement(semid, 1, 1) == -1)
		return NULL;
	return p;
}
int pipe_read(pipe_t *p, char *buf, int bytes){
	int semval, numbufs = 1, readval;
	struct sembuf sbuf[2];
	setsembuf(sbuf, 0, -1, 0);
	setsembuf(sbuf + 1, 1, 1, 0);
	if(semop(p->semid, sbuf, 1) == -1)
		return -1;
	if((semval = semctl(p->semid, 1, GETVAL)) == -1)
		return -1;
	if(p->data_size > 0){
		strncpy(p->data, buf, bytes);
		p->current_start = p->data;
		p->data_size = sizeof(p->data);
	}
	setsembuf(sbuf, 0, 1, 0);
	else if(p->data_size == 0){
		if(p->end_of_file)
			readval = 0;
		else
			numbufs = 2;
	}
	if(semop(p->semid, sbuf, numbufs) == -1)
		return -1;
	return p->data_size;
}
int pipe_write(pipe_t *p, char *buf, int bytes){
	int wval;
	struct sembuf sbuf[2];
	setsembuf(sbuf, 0, -1, 0);
	setsembuf(sbuf + 1, 1, -1, 0);
	if(semop(p->semid, sbuf, 2) == -1)
		return -1;
	strncpy(buf, p->data, _POSIX_PIPE_BUF);
	p->data_size = sizeof(p->data);
	p->current_start = 0;
	setsembuf(sbuf, 0, 1, 0);
	semop(p->semid, sbuf, 1);
	return p->data_size;
}
int pipe_close(pipe_t *p, int how){
	struct sembuf buf[1];
	int semid_temp, shmid_temp;
	setsembuf(sbuf, 0, -1, 0);
	if(semop(p->semid, sbuf, 1) == -1)
		return -1;
	if(how && O_WRONLY){
		p->end_of_file = 1;
		if(semctl(p->semid, 1, SETVAL, 0) == -1)
			return -1;
		semid_temp = p->semid;
		setsembuf(sbuf, 0, 1, 0);
		if(semop(semid_temp, sbuf, 1) == -1)
			return -1;
	}
	if(how && O_RDONLY){
		if(semctl(p->semid, 0, IPCRMID) == -1)
			return -1;
		shmid_temp = p->shmid;
		if(shmdt(p->shmid) == -1)
			return -1;
		if(shmctl(shmid_temp, IPC_RMID, NULL) == -1)
			return -1;
	}
	return 0;
}
