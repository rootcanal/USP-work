#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef MAX_CANON
	#define MAX_CANON 256
#endif

int makeargv(const char *s, const char *delimiters, char ***argvp);

int main(int argc, char *argv[]){
	char in[MAX_CANON], delim[] = " ", **myargv;
	int pr_limit, pr_count = 0, child, w;

	if(argc != 2){
		fprintf(stderr, "Usage:%s arg\n", argv[0]);
		return 1;
	}

	pr_limit = atoi(argv[1]);

	while(fgets(in, MAX_CANON, stdin)){
		if(pr_count == pr_limit){
			wait(NULL);
			pr_count--;
		}



		child = fork();
		if(child == -1){
			perror("failed to fork");
			return 1;
		}
		if(child == 0){
			if(makeargv(in, delim, &myargv) == -1){
				perror("child failed to construct arg array");
			}else{
				fprintf(stderr, "myargv: %s, %s, %s\n", myargv[0], myargv[1], myargv[2]);
				execvp(myargv[0], &myargv[0]);
				perror("child failed to exec");
			}
			return 1;
		}
		pr_count++;
		w = waitpid(-1, NULL, WNOHANG);
		if(w == -1){
			perror("wait failed");
			return 1;
		}
		pr_count--;
	}
	return 0;
}

