#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void wastesometime(int n){
	static volatile int dummy = 0;
	int i;
	for(i = 0; i < n; i++)
		dummy++;
}

void prtastr(const char *s, int fd, int n){
	char *c;
	int meh;
	for (c = s; c < s + strlen(s); c++)	{
		meh = write(fd, c, 1);
		wastesometime(n);
	}
}

int main(int argc, char *argv[]){
	pid_t childpid;
	int error;
	int fd[2];
	int i;
	long nlocks = 0, cmplock;
	double drand;
	int nprocs;
	char *nxt, *s = malloc(sizeof(long)*20), *t = malloc(sizeof(long)*20);

	fd[0] = -1; fd[1] = -1;
	if( (argc != 4) || ((nprocs = atoi(argv[1])) <= 0)) {
		fprintf(stderr, "Usage: %s nprocs time_waste rand_max\n", argv[0]);
		return 1;
	}
	if ((nprocs = atoi (argv[1])) <= 0) {
       fprintf (stderr, "Usage: %s nprocs\n", argv[0]);
       return 1;
   }
	if(pipe(fd) == -1){
		perror("Failed to create starting pip");
		return 1;
	}
	if((dup2(fd[0], STDIN_FILENO) == -1) ||
		(dup2(fd[1], STDOUT_FILENO) == -1)) {
		perror("Failed to connect pipe");
		return -1;
	}
	if((close(fd[0] == -1) || close(fd[1]) == -1)){
		perror("Failed to close extra descriptors");
		return 1;
	}
	for(i = 1; i < nprocs; i++){
		if(pipe(fd) == -1){
			fprintf(stderr, "[%ld]: failed to create pipe %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
		if((childpid = fork()) == -1){
			fprintf(stderr, "[%ld]: failed to create child %d: %s\n",(long)getpid(), i, strerror(errno));
			return 1;
		}
		if(childpid > 0)
			error = dup2(fd[1], STDOUT_FILENO);
		else
			error = dup2(fd[0], STDIN_FILENO);
		if (error == -1){
			fprintf(stderr, "[%ld]: failed to dup pipes for iteration %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
		if ((close(fd[0]) == -1 || close (fd[1]) == -1)) {
			fprintf(stderr, "[%ld]: failed to close extra descriptors %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
		if(childpid != 0)
			 break;
	}
	if(i == 1){
		sprintf(t, "%ld %ld", nlocks, (long)getpid());
		write(STDOUT_FILENO, t, sizeof(t));
		fprintf(stderr, "parent done writing\n");
	}
	sprintf(s, "This is process %d with ID equal to %ld and parent id %ld\n",
		i, (long)getpid(), (long)getppid());
	while(nlocks < 10){
		if(nlocks >0)
			fprintf(stderr, "woot\n");
		read(STDIN_FILENO, t, sizeof(t));
		if(i == 1 || i == 2)
			fprintf(stderr, "%ld is cmplock\n", cmplock);
		cmplock = strtol(t, &nxt, 10);
		if((nlocks < cmplock) || (nlocks == cmplock && strtol(t, &nxt, 10) <= (long)getpid())){
			nlocks++;
			sprintf(t, "%ld %ld", nlocks, (long)getpid());
			srand48((long)getpid());
			drand = drand48()*atoi(argv[3]);
			fprintf(stderr, "%f\n",drand);
			while(drand-- > 0)
				prtastr(s, STDERR_FILENO, atoi(argv[2]));
		}
	}
	write(STDOUT_FILENO, t, sizeof(t));
	return 0;
}
