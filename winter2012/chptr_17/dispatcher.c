#include "ntpvm.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){
	int comp_id, task_id, length, pipefd[2], child;
	packet_t type;
	unsigned char buf[MAX_PACK_SIZE];
	char **argz, chout[MAX_PACK_SIZE];
	ntpvm_task_t *task = malloc(sizeof(ntpvm_task_t));

	if(getpacket(STDIN_FILENO, &comp_id, &task_id, &type,
               &length, buf)) {
		putpacket(STDOUT_FILENO, comp_id, task_id, DONE, 7, "WATSOUP");
		return 0;
	}
	task->compid = comp_id;
	task->taskid = task_id;
	task->recvbytes = length;

	if(type != NEWTASK){
		fprintf(stderr, "nota newtask->wat\n");
		return -1;
	}
	if(pipe(pipefd) == -1){
		perror("pipe");
		return -1;
	}
	if(dup2(pipefd[0], task->writefd)== -1 || dup2(pipefd[1], task->readfd) == -1){
		perror("failed to dup my parent fds");
		return -1;
	}
	if((child = fork()) == -1){
		perror("fork");
		return -1;
	}
	if(!child){
		if(dup2(pipefd[0], STDIN_FILENO) == -1){
			perror("child failed on stdindup");
			return -1;
		}
		if(dup2(pipefd[1], STDOUT_FILENO) == -1){
			perror("child failed on stdoutdup");
			return -1;
		}
		if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1){
			perror("closing extra fds");
			return -1;
		}
		if(makeargv(buf, " ", &argz) == -1){
			perror("makeargv");
			return -1;
		}
		if(execvp(argz[0], argz) == -1){
			perror("execvp");
			return -1;
		}
		exit(0);
	}
	else{

/*		close(pipefd[0]);
		close(pipefd[1]);
*/
		if(readblock(task->readfd, chout, MAX_PACK_SIZE) == -1){
			perror("parent readblock");
			return -1;
		}
		putpacket(STDOUT_FILENO, task->compid, task->taskid, DATA, strlen(chout), chout);
		wait(NULL);
	}
}
