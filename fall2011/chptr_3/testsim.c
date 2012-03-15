#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	int sleep_time = atoi(argv[1]), repeat = atoi(argv[2]);

	while(repeat--){
		sleep(sleep_time);
		fprintf(stderr, "%i\n", getpid());
	}
	return 0;
}
