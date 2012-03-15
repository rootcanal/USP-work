#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "restart.h"

int waitatbarrier(char *name){
	int reqfd, relfd;
	char buf[2];
	char request[sizeof(name)+sizeof(".request")+1];
	char release[sizeof(name)+sizeof(".release")+1];

	sprintf(request, %s%s, name, ".request");
	sprintf(release, %s%s, name, ".release");

	while(1){

		if((reqfd = open(request, O_WRONLY)) == -1){
			perror("failed to open req pipe");
			return 1;
		}
		if (r_read(reqfd, buf, 1) != 1){
			perror("Failed to read synchronization characters");
			return 1;
		}
		if (close(reqfd) == -1){
			perror("failed to close req pipe");
			return 1;
		}
		if((relfd = open(request, O_RDONLY)) == -1){
			perror("failed to open rel pipe");
			return 1;
		}
		if (r_write(relfd, buf, 1) != 1){
			perror("Failed to write synchronization characters");
			return 1;
		}
		if (close(relfd) == -1){
			perror("failed to close rel pipe");
			return 1;
		}
	}
}

int main  (int argc, char *argv[]) {
	char *request, *release, *buf;
	pid_t childpid = 0;
	int barsize;
	int i, n, reqfd, relfd;

	if (argc != 3){      /* check for valid number of command-line arguments */
      fprintf (stderr, "Usage: %s name barrier_size\n", argv[0]);
      return 1;
   }
    name = argv[1];
	barsize = atoi(argv[2]);
	buf = malloc(sizeof(barsize) + 1);

	request = malloc(sizeof(argv[2]) + sizeof(".request") +1);
	sprintf(request, %s%s,argv[2],".request");
   if (mkfifo(request, FIFO_PERM) == -1) {
      if (errno != EEXIST) {
         fprintf(stderr, "[%ld]:failed to create named pipe %s: %s\n",
              (long)getpid(), request, strerror(errno));
         return 1;
      }
   }
	release = malloc(sizeof(argv[2]) + sizeof(".release") +1);
	sprintf(release, %s%s, argv[2], ".release");
   if (mkfifo(release, FIFO_PERM) == -1) {
      if (errno != EEXIST) {
         fprintf(stderr, "[%ld]:failed to create named pipe %s: %s\n",
              (long)getpid(), release, strerror(errno));
         return 1;
      }
   }
	while(1){
		if((reqfd = open(request, O_RDONLY)) == -1){
			perror("failed to open req pipe");
			return 1;
		}
		if (r_read(reqfd, buf, argv[2]) != 1){
			perror("Failed to read synchronization characters");
			return 1;
		}
		if (close(reqfd) == -1){
			perror("failed to close req pipe");
			return 1;
		}
		if((relfd = open(request, O_WRONLY)) == -1){
			perror("failed to open rel pipe");
			return 1;
		}
		if (r_write(relfd, buf, argv[2]) != 1){
			perror("Failed to write synchronization characters");
			return 1;
		}
		if (close(relfd) == -1){
			perror("failed to close rel pipe");
			return 1;
		}
	}
}
