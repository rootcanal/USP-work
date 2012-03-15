#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CHUNK 256

/*This code does not work.*/

int processchar(int fdin, int fdout, char inchar, char *outstr){

	char buf[CHUNK];
	int r, outlen = strlen( outstr );

	while ( (r = read( fdin, buf, CHUNK )) > 0){
		int i;
		for ( i = 0; i < r; i++){
			if(buf[i] == inchar ){
				if(write(fdout, outstr, outlen ) < 0 ) return -1;
			} else {
				if( write(fdout, &buf[i], 1) < 0 ) return -1;
			}
		}
		fprintf(stderr, "buf is %s\n", buf);
	}
	return r;
}


/*	int j = 0;
	char *buf, *tmp_buf, *fin_buf, *k, *rbuf, *rrbuf;
	int n = CHUNK, np = CHUNK, r;



	if((buf = malloc(sizeof(char) * n)) == NULL)
		return 1;
	while((r = read(fdin, buf, CHUNK))){
		if( r == -1)
			return -1;
		n += r;
		if(np - n < CHUNK) {
			np *= 2;
			rbuf = malloc(np*sizeof(char));
			memcpy(rbuf, buf, n * sizeof(char));
			free(buf);
			buf = rbuf;
		}
	}
	fprintf(stderr, "buf is %s\n", buf);
	for(tmp_buf = buf; tmp_buf < buf + n; tmp_buf++){
		if(*tmp_buf == inchar)
			j++;
	}
	if((fin_buf = malloc((sizeof(char) * n) +
			(sizeof(char) * j * strlen(outstr) + 2)) == NULL)){
		perror("fin_buf didn't malloc rite:");
		return 1;
	}
	rrbuf = fin_buf;
	for(tmp_buf = buf; tmp_buf < buf + n; tmp_buf++){
	fprintf(stderr, " *tmp_buf%c\n", *tmp_buf);
		if (*tmp_buf == inchar){
			fprintf(stderr, "found a j as pid: %ld\n", (long)getpid());
			k = outstr;
			while((*fin_buf++ = *k++));
		}else{
			fprintf(stderr, "doin somehtin\n");
			*fin_buf++ = *tmp_buf;
		}
	}
	fprintf(stderr, "%s\n",fin_buf);
	write(fdout, rrbuf, strlen(rrbuf));
	return 0;
}
*/
int main(int argc,  char *argv[ ]) {
   pid_t childpid;
	int error;
	int fd[2];
	int i = 0;
	int n, fild, read_size;
	FILE * fp;
	struct stat sb;
	char buf[1024], **conf, *fildstr;

	fd[0] = -1; fd[1] = -1;
          /* check command line for a valid number of processes to generate */
   if  (argc != 5)  {
       fprintf (stderr, "Usage: %s n conf.in file.in file.out\n", argv[0]);
       return 1;
   }
	if ((n = atoi (argv[1])) <= 0) {
       fprintf (stderr, "Usage: %s num conf.in file.in file.out\n", argv[0]);
       return 1;
   }
	conf = malloc(sizeof(char *) * n);
	if(!(fp = fopen(argv[2], "r")))
		return 1;
	while(fgets(buf, sizeof(buf), fp) != NULL && i < n){
		conf[i] = malloc(sizeof(buf));
		strcpy(conf[i++], buf);
	}
	if(i < n){
		fprintf(stderr, "wat small/big conf file. don't know watdo\n");
		return -1;
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
   for (i = 0; i < n;  i++) {         /* create the remaining processes */
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
      if (childpid == 0)
         break;
   }
	if(childpid > 0){
		if(stat(argv[3], &sb) == -1)
			return 1;
		fildstr = malloc(sizeof(char) * (long long)sb.st_size);
		if((fild = open(argv[3], O_RDONLY)) > 0){
			if((read_size = read(fild, fildstr, sb.st_size)) == -1){
				perror("parent is cant read");
				return 1;
			}
			if(write(STDOUT_FILENO, fildstr, read_size) != read_size){
				fprintf(stderr, "parent is write bad\n");
				return 1;
			}
			close(fild);
		}
		else{
			return 1;
		}
	} else if (i == n - 1){
		sleep(i);
		fprintf(stderr, "made it to the last one\n");
		if(stat(argv[4], &sb) == -1)
			fild = open(argv[4], O_CREAT);
		else
			fild = open(argv[4], O_WRONLY);
		fildstr = malloc(1024);
		while((i = read(STDIN_FILENO, fildstr, 1024)) > 0)
			write(fild, fildstr, i);
	} else {
		char *lol = strtok(conf[i], " "), *sup = strtok(NULL, " ");
		fprintf(stderr, "*lol, sup: %c %s\n", *lol, sup);
		sleep(i);
		if(processchar(STDIN_FILENO, STDOUT_FILENO, *lol, sup)){
			return 1;
		}
	}
   return 0;
}
