#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

extern char **environ;

void print_environ(){
	int i;
	for(i = 0; environ[i] != NULL; i++){
		printf(" %s\n", environ[i]);
	}
}

void clear_env(){
	environ = NULL;
}

void change_envs(char **argv, int i){
	char * str;

	while((str = strtok(argv[i++], "=")) != NULL){
		setenv(str, strtok(NULL, "="), 1);
	}
}

void run_it(char **argv, int argc, int i){
	int j = 0;
	char *utility = argv[i];
	char *util_argv[argc-i];

	util_argv[j++] = utility;

	while(argv[++i] != NULL){
		util_argv[j++] = argv[i];
	}
	util_argv[j] = NULL;
	execvp(utility, util_argv);
}

int main(int argc, char *argv[]) {
	int env_cleared = 0; int i = 1;

	if(argc == 1) {
		print_environ();
		return 0;
	}
	if(strcmp(argv[1], "-i") == 0){
		clear_env();
		env_cleared = 1;
		i++;
	}
	if(argc > 2 || (argc > 1 && env_cleared == 1)){
		change_envs(argv, i);
	}
	if(i < argc){
		run_it(argv, argc, i);
	}
	return 0;
}
