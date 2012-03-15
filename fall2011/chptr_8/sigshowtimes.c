#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>

/* for simplification I truncated the doubles to ints before converting to strings*/

static char * double_to_str(double d){
	int a = 0, e = (int)d;
	char *b;
	while(e){
		e /= 10;
		a++;
	}
	b = malloc(sizeof(char)*a);
	e = (int)d;
	while(a--){
		b[a] = (char)(((int)'0')+e%10);
		e /= 10;
	}
	return b;
}

static void showtimeshandler(int signo) {
	int serrno;
	double ticks;
	struct tms tinfo;
	char *d;

	serrno = errno;

	if ((ticks = (double) sysconf(_SC_CLK_TCK)) == -1)
		write(STDERR_FILENO, "Failed to determine clock ticks per second\n", 43);
	else if (times(&tinfo) == (clock_t)-1)
		write(STDERR_FILENO, "Failed to get times information\n", 33);
	else {
		d = double_to_str(tinfo.tms_utime/ticks);
		write(STDERR_FILENO, "User time:	",11);
		write(STDERR_FILENO, d, sizeof(d));
		write(STDERR_FILENO, " seconds\n", 9);

		d = double_to_str(tinfo.tms_stime/ticks);
		write(STDERR_FILENO, "System time:	",13);
		write(STDERR_FILENO, d, sizeof(d));
		write(STDERR_FILENO, " seconds\n", 9);

		d = double_to_str(tinfo.tms_cutime/ticks);
		write(STDERR_FILENO, "Children's user time:	",22);
		write(STDERR_FILENO, d, sizeof(d));
		write(STDERR_FILENO, " seconds\n", 9);
	}
	errno = serrno;
}

int main() {

	int signum = SIGUSR1;
	struct sigaction *act = NULL;

	volatile int i = 0;

	sigaction(signum, act, act);

	act->sa_handler = showtimeshandler;
	act->sa_flags = 0;
	if((sigemptyset(&act->sa_mask) == -1) ||
		 (sigaction(signum, act, NULL) == -1) ){
		perror("failed to set SIGUSR1 signal handler");
		return 1;
	}

	fprintf(stderr, "process %ld looping for SIGUSR1\n", (long)getpid());

	for ( ; ; ) {
		i++;
	}

	 /*  rest of main program goes here */
	return 0;
}
