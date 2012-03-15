#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"
#include "doneflag.h"
#include "globalerror.h"

int initconsumer(pthread_t *tid);
int initproducer(pthread_t *tid, void *args);
int showresults(void);

int main(int argc, char *argv[]) {
	int error;
	int i;
	int numberconsumers;
	pthread_t *tidc, tidp;
	struct timeval tv;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s sleeptime producers consumers\n", argv[0]);
		return 1;
	}
	numberconsumers = atoi(argv[1]);
	tidp = (pthread_t)calloc(1, sizeof(pthread_t));
	tidc = (pthread_t *)calloc(numberconsumers, sizeof(pthread_t));
	if (tidc == NULL) {
		perror("Failed to allocate space for consumer IDs");
		return 1;
	}
	if(gettimeofday(&tv, NULL)){
		perror("wat on gettimeofday");
		return -1;
	}
	fprintf(stderr, "STARTING AT %ld\n", tv.tv_sec);
	for (i = 0; i < numberconsumers; i++){				 /* initialize consumers */
		if (error = initconsumer(tidc+i)) {
		  fprintf(stderr, "Failed to create consumer %d:%s\n",
							i, strerror(error));
			return 1;
		}
	}
	if(error = initproducer(&tidp, argv+2)){
		fprintf(stderr, "Failed to create producer\n");
		return 1;
	}
	if (error = pthread_join(tidp, NULL)) {
		fprintf(stderr, "Failed producer %d join:%s\n", i, strerror(error));
		return 1;
	}
	for (i = 0; i < numberconsumers; i++){					/* wait for consumers */
		if (error = pthread_join(tidc[i], NULL)) {
			fprintf(stderr, "Failed consumer %d join:%s\n", i, strerror(error));
			return 1;
		}
	}
	if(gettimeofday(&tv, NULL)){
		perror("wat on gettimeofday");
		return -1;
	}
	fprintf(stderr, "ENDING AT %ld\n", tv.tv_sec);
	return 0;
}
