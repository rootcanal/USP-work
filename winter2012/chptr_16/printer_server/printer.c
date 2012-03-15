#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "globalerror.h"
#include "prcmd.h"

void *printer(void *arg){
	int *fd = (int *)arg, error, wfd;
	prcmd_t *prcmd;

	for( ; ; ){
		if(error = rremove(&prcmd))
			break;
		if(wfd = open(prcmd->filename, O_RDONLY)){
			error = wfd;
			continue;
		}
		copyfile(wfd, *fd);
	}
	if (error)
		seterror(error);
}
/* --------------- Public functions ---------------------------------------- */
int initconsumer(pthread_t *tconsumer) {                       /* initialize */
   int error;

   error = pthread_create(tconsumer, NULL, printer, NULL);
   return (seterror(error));
}
