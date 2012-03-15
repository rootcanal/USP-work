#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "restart.h"
#define BUFSIZE 1024

int getnamed(char *name, sem_t **sem, int val);
int destroynamed(char *name, sem_t *sem);
static int n;
static char name[BUFSIZE];
static sem_t *semlockp;

static void docommand(char *cmd){
	int childpid;
	char **argvp;

	if((childpid = fork()) == -1)
		fprintf(stderr, "failed at docommandfork\n");
	if(childpid){
		waitpid(childpid, NULL, NULL);
		returnlicense();
	}
	else{
		makeargv(buf, " ", &argvp);
		execvp(argvp[0], argvp);
	}
}
int initlicense(void){
	if(snprintf(name, BUFSIZE, "/tmp.license.%d\0", getpid()) >= 0){
		fprintf(stderr, "failed to snprintfininitlicense\n");
		return -1;
	}
	if(getnamed(name, &semlockp, n) == -1){
		fprintf(stderr, "failed to getnamed in initlicense\n");
		return -1;
	}
}
int getlicense(void){
	while(sem_wait(semlockp) == -1)
		if(errno != EINTR) {
			perror("Failed to lock semlock");
			return 1;
		}
}
int returnlicense(void){
	if(sem_post(semlockp) == -1) {
		perror("Failed to unlock semlock");
		return 1;
	}
}
int addtolicense(void){

}
int removelicenses(int num){

}
int main  (int argc, char *argv[]) {
	char buf[BUFSIZE];
	int childpid;

	if(argc != 2){
		fprintf(stderr, "usage: runsim <numprocs>\n");
		return -1;
	}
	n = argv[1];
	if(initlicense()){
		fprintf(stderr, "failed to initlicense\n");
		return -1;
	}
	while(readline(STDIN_FILENO, buf, BUFSIZE-1) != -1){
		if(strcmp(EOF, buf))
			return -1;
		if(getlicense() == -1){
			fprintf(stderr, "failed on getlicense\n");
			return -1;
		}
		if((childpid = fork()) == -1){
			fprintf(stderr, "failed at fork\n");
			return -1;
		}
		if(childpid)
			waitpid(childpid, NULL, WNOHANG);
		else{
			docommand(buf);
			exit(1);
		}
	}
	return (destroynamed(name, &semlockp));
}
