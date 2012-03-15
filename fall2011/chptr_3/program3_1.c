#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	pid_t childpid = 0;
	int i, n, j;
	char mybuf[atoi(argv[4])];

	if(argc != 5){
		fprintf(stderr, "usage: %s processes k m nchars\n", argv[0]);
		return 1;
	}

	n = atoi(argv[i]);
	for(i = 1; i < n; i++){
		if((childpid = fork()))
			break;
	}

	sleep(10);
	for(i = 0; i < atoi(argv[2]); i++){
		sleep(atoi(argv[3]));
		wait(NULL);
		fprintf(stderr, "i:%d process ID:%ld", i, (long)getpid());
		fprintf(stderr, "i:%d parent ID:%ld", i, (long)getppid());
		fprintf(stderr, "i:%d child ID:%ld\n", i, (long)childpid);
		for(j = 0; j < atoi(argv[4]); j++)
			mybuf[j] = getchar();
		mybuf[atoi(argv[4])] = '\0';
		fprintf(stderr, "%d:%s",getpid(), mybuf);
	}
	return 0;
}
