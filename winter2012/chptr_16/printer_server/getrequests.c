#include <errno.h>
#include "prcmd.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "globalerror.h"

void *getrequests(void *arg){
	int *fd =(int *)arg, bytes;
	char infile[MAX_PATH+sizeof(int)+1];
	char *owner;
	prcmd_t *prcmd;
	for( ; ; ){
		if( read(*fd, infile, (MAX_PATH + sizeof(int)) + 1) == 0){
			seterror(ECANCELED);
			return;
		}
		owner = strtok(infile, " ");
		if((prcmd = malloc(sizeof(prcmd_t))) == NULL){
			seterror(ECANCELED);
			return;
		}
		prcmd->owner = atoi(owner);
		strcpy(prcmd->filename, strtok(NULL, " "));
		if(add(prcmd)){
			seterror(ECANCELED);
			return;
		}
	}
}
int initproducer(pthread_t *tproducer, void *args){
	int error;
	error = pthread_create(tproducer, NULL, getrequests, args);
	return (seterror(error));
}
