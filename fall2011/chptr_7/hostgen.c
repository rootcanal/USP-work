#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "restart.h"

int hostgen(int proc_num, int sleep_time){
	char buf[80];
	int rand;

	srand48(getpid());
	rand = lrand48() % sleep_time;

	if(readtimed(STDIN_FILENO, buf, 80, rand) == -1)
		fprintf(stdout, "%ld\n", lrand48() % proc_num);
	write(STDERR_FILENO, buf, 80);
	return 0;
}
