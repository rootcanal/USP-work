#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int main(int argc,  char *argv[ ]) {
   pid_t childpid;             /* indicates process should spawn another     */
   int error;                  /* return value from dup2 call                */
   int fd[2];                  /* file descriptors returned by pipe          */
   int i;
	long j, k;                      /* number of this process (starting with 1)   */
   int nprocs;                 /* total number of processes in ring          */
	char *fib_in = malloc(sizeof(long)*2+3);
	char *fib_ptr = malloc(sizeof(long)*2+3);
	char *fib_out = malloc(sizeof(long)*2+3);

	fd[0] = -1; fd[1] = -1;
          /* check command line for a valid number of processes to generate */
   if  (argc != 2)  {
       fprintf (stderr, "Usage: %s nprocs\n", argv[0]);
       return 1;
   }
	if ((nprocs = atoi (argv[1])) <= 0) {
       fprintf (stderr, "Usage: %s nprocs\n", argv[0]);
       return 1;
   }
   if (pipe (fd) == -1) {      /* connect std input to std output via a pipe */
      perror("Failed to create starting pipe");
      return 1;
   }
   if ((dup2(fd[0], STDIN_FILENO) == -1) ||
       (dup2(fd[1], STDOUT_FILENO) == -1)) {
      perror("Failed to connect pipe");
      return 1;
   }
   if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) {
      perror("Failed to close extra descriptors");
      return 1;
   }
   for (i = 1; i < nprocs;  i++) {         /* create the remaining processes */
      if (pipe (fd) == -1) {
         fprintf(stderr, "[%ld]:failed to create pipe %d: %s\n",
                (long)getpid(), i, strerror(errno));
         return 1;
      }
      if ((childpid = fork()) == -1) {
         fprintf(stderr, "[%ld]:failed to create child %d: %s\n",
                 (long)getpid(), i, strerror(errno));
         return 1;
      }
      if (childpid > 0)               /* for parent process, reassign stdout */
          error = dup2(fd[1], STDOUT_FILENO);
      else                              /* for child process, reassign stdin */
          error = dup2(fd[0], STDIN_FILENO);
      if (error == -1) {
         fprintf(stderr, "[%ld]:failed to dup pipes for iteration %d: %s\n",
                 (long)getpid(), i, strerror(errno));
         return 1;
      }
      if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) {
         fprintf(stderr, "[%ld]:failed to close extra descriptors %d: %s\n",
                (long)getpid(), i, strerror(errno));
         return 1;
      }
		if ((close(fd[0]) == -1 || close (fd[1]) == -1)) {
			fprintf(stderr, "[%ld]: failed to close extra descriptors %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
      if (childpid != 0)
         break;
   }                                               /* say hello to the world */
	if (i == 1){
		sprintf(fib_out, "%ld %ld ", (long)1, (long)1);
		write(STDOUT_FILENO, fib_out, sizeof(fib_out));
		read(STDIN_FILENO, fib_in, sizeof(fib_in));
	}
	else {
		read(STDIN_FILENO, fib_in, sizeof(long)*2+4);
		fprintf(stderr, "%s\n", fib_in);
		k = strtol(fib_in, &fib_ptr, 10);
		j = strtol(fib_ptr, NULL, 10);
		k = k+j;
		sprintf(fib_out, "%ld %ld ", j, k);
		write(STDOUT_FILENO, fib_out, sizeof(long)*2+4);
	}
   fprintf(stderr, "This is process %d with ID %ld and fib_out %s\n",
           i, (long)getpid(), fib_out);
   return 0;
}
