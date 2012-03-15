#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#ifndef max
	#define max(a, b)( ((a) > (b)) ? (a) : (b) )
#endif

typedef enum msg_type{TOKEN, HOST2HOST, IMP2HOST, HOST2IMP, IMP2IMP} msg_type;

typedef enum msg_status{NONE, NEW, ACK} msg_status;

typedef struct {
	msg_type type;
	long source_id;
	long dest_id;
	msg_status status;
	long msg_num;
} msg;

int main(int argc,  char *argv[ ]) {
   pid_t childpid;             /* indicates process should spawn another     */
   int error;                  /* return value from dup2 call                */
   int fd[2], imp1fd[2], imp2fd[2];/* file descriptors returned by pipe      */
   int i;                      /* number of this process (starting with 1)   */
   int nprocs;                 /* total number of processes in ring          */
	fd_set rfds, wfds;
	struct timeval tv;
	int retval, nfds, got_msg;
	char buf_tmp[1024];
	msg msg_tmp;

	fd[0] = -1;
	fd[1] = -1;

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
      if (childpid)
         break;
   }                                               /* say hello to the world */

	if(pipe(imp1fd) == -1){
		fprintf(stderr, "[%ld]:failed to create pipe for IMP: %s\n",
			(long)getpid(), strerror(errno));
		return 1;
	}
	if(pipe(imp2fd) == -1){
		fprintf(stderr, "[%ld]:failed to create pipe for IMP: %s\n",
			(long)getpid(), strerror(errno));
		return 1;
	}
	if ((childpid = fork()) == -1){
		fprintf(stderr, "[%ld]: failed to create child for IMP: %s\n",
			(long)getpid(), strerror(errno));
		return 1;
	}
	if (childpid == 0){
		dup2(imp1fd[1], STDOUT_FILENO);
		dup2(imp2fd[0], STDIN_FILENO);
	}
	if ((close(imp2fd[0]) == -1) || (close(imp1fd[1]) == -1)) {
		fprintf(stderr, "[%ld]:failed to close extra descriptors %d: %s\n",
			(long)getpid(), i, strerror(errno));
		return 1;
	}
	if(childpid){
		got_msg = 0;
		while(1){
			FD_ZERO(&rfds);
			FD_SET(STDIN_FILENO, &rfds);
			FD_SET(imp1fd[0], &rfds);

			FD_ZERO(&wfds);
			FD_SET(STDOUT_FILENO, &wfds);
			FD_SET(imp2fd[1], &wfds);

			tv.tv_sec = 0;
			tv.tv_usec = 0;

			nfds = max(imp1fd[0], imp2fd[1]) + 1;
			retval = select(nfds, &rfds, &wfds, NULL, &tv);
			if(retval == -1)
				perror("select()");
			else if(retval){
				if(FD_ISSET(imp1fd[0], &rfds) && got_msg == 0){
					read(imp1fd[0], buf_tmp, 1024);
					got_msg = 1;
				}
				if(FD_ISSET(STDIN_FILENO, &rfds)){
					read(STDIN_FILENO, &msg_tmp, sizeof(msg_tmp));
					if(msg_tmp.type == TOKEN){
						if(got_msg){
							got_msg = 0;
							msg_tmp.type = HOST2IMP;
							msg_tmp.source_id = getpid();
							msg_tmp.dest_id = strtol(buf_tmp, NULL, 10);
							msg_tmp.status = NEW;
							msg_tmp.msg_num = 0;
						}
						write(STDOUT_FILENO, &msg_tmp, sizeof(msg_tmp));
					} else {
						if(msg_tmp.source_id == getpid()){
							fprintf(stderr, "process %ld sent msg %ld to destination %ld with status %ld\n",
								msg_tmp.source_id, msg_tmp.msg_num, msg_tmp.dest_id, msg_tmp.msg_num);
							msg_tmp.type = TOKEN;
							write(STDOUT_FILENO, &msg_tmp, sizeof(msg_tmp));
						}
						if(msg_tmp.dest_id == getpid()){
							fprintf(stderr, "process %ld sent me a msg %ld to destination %ld with type %d\n",
								msg_tmp.source_id, msg_tmp.msg_num, msg_tmp.dest_id, msg_tmp.type);
							msg_tmp.status = ACK;
							write(STDOUT_FILENO, &msg_tmp, sizeof(msg_tmp));
							if(msg_tmp.type == IMP2HOST || msg_tmp.type == HOST2HOST)
								write(imp2fd[1], &msg_tmp, sizeof(msg_tmp));
						}
					}
				}
				if(FD_ISSET(STDOUT_FILENO, &wfds))
					;
				if(FD_ISSET(imp2fd[1], &wfds))
					;
			}
		}
	} else
		hostgen((int)getpid(), 10);

   fprintf(stderr, "This is process %d with ID %ld and parent id %ld\n",
           i, (long)getpid(), (long)getppid());
   return 0;
}
