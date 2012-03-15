#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc,  char *argv[ ]) {
   pid_t childpid;             /* indicates process should spawn another     */
   int error;                  /* return value from dup2 call                */
   int fd[2];                  /* file descriptors returned by pipe          */
   int i,m,n,x,active,nactive; /* number of this process (starting with 1)   */
   int nprocs;                 /* total number of processes in ring          */
	char *in[1];

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
      if (childpid != 0)
         break;
   }                                               /* say hello to the world */
   fprintf(stderr, "This is process %d with ID %ld and parent id %ld\n",
           i, (long)getpid(), (long)getppid());
	m = n;
	active = 1;
	nactive = 0;
	srand48(getpid());
	while(active){
		x = lrand48() % m;
		if(x == 1){
			write(STDOUT_FILENO, "1", 1);
			nactive++;
		}
		else
			write(STDOUT_FILENO, "0", 1);
		i = 0;
		while(i++ < n){
			read(STDIN_FILENO, in, 1);
			if(strncmp("1", in, 1) == 0)
				nactive++;
		}
		if(nactive == 1)
			fprintf(stderr, "election complete\n");
		else if(nactive == 0)
			;
		else if(active && x == 1)
			active = 0;
	}
   return 0;
}