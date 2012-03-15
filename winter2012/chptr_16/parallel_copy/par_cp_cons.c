#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include "buffer.h"
#include "globalerror.h"
#include "../../fall2011/chapter04/restart.h"

static void *consumer(void *wat){
	int error;
	buffer_t item;
	for( ; ; ){
		if(error = getitem(&item))
			break;
		if((error = copyfile(item.infd, item.outfd)) == -1)
			break;
		printf("cp'ed file %s\n", item.filename);
	}
}
/* --------------- Public functions ---------------------------------------- */
int initconsumer(pthread_t *tconsumer) {                       /* initialize */
   int error;

   error = pthread_create(tconsumer, NULL, consumer, NULL);
   return (seterror(error));
}
