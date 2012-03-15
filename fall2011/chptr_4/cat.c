#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "restart.h"

static int bufsize = 1024;

int do_fdwrite(fd){
	int i;
	char buf[bufsize];
	while((i = r_read(fd, buf, bufsize))){
		if(r_write(STDOUT_FILENO, buf, i) == -1)
			return -1;
	}
	return 0;
}

int main(int argc, char *argv[]){
	int i = 0, fd, j = 0;

	if(argc == 1){
		if(do_fdwrite(STDIN_FILENO)){
			perror("failed to do_stdin");
			return -1;
		}
	}
	if(!strcmp(argv[1], "-u")){
		bufsize = 2;
		if(argc == 2){
			if(do_fdwrite(STDIN_FILENO)){
				perror("failed to do_stdin");
				return -1;
			}
		}
		i++;
	}
	while(++i < argc){
		if(!strcmp(argv[i], "-")){
			if(do_fdwrite(STDIN_FILENO)){
				perror("failed to do_stdin");
				return -1;
			}
			break;
		}
		if((fd = open(argv[i], O_RDONLY)) == -1){
			perror("failed to open");
			return -1;
		}
		if((j = do_fdwrite(fd)) == -1){
			perror("readwrite error");
			return -1;
		}
	}
	return 0;
}
