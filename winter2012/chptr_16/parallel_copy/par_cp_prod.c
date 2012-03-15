#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "buffer.h"
#include "globalerror.h"

static void *producer(char *dirnames[2]){
	DIR *rdir, *wdir;
	FILE *rfile, *wfile;
	char rfname[MAXNAME_SIZE], wfname[MAXNAME_SIZE];
	struct dirent *entry;
	struct stat *sbuf;
	buffer_t item;
	int err;
	if((rdir = opendir(dirnames[0])) == NULL || (wdir = opendir(dirnames[1])) == NULL)
		printf("watsouped all wrong \n");
	while((entry = readdir(rdir)) != NULL){
		if(lstat(entry->d_name, sbuf)){
			printf("naw this aint workeded\n");
			break;
		}
		if(!S_ISREG(sbuf->st_mode)){
			printf("derped on that one for sure\n");
			break;
		}
		if(strlen(entry->d_name)+strlen(dirnames[0]) >= MAXNAME_SIZE){
			printf("cant read that deep\n");
			break;
		}
		sprintf(rfname, "%s%s",dirnames[0], entry->d_name);
		if((rfile = fopen(rfname, "r")) == NULL){
			printf("reading wrongeded\n");
			break;
		}
		if((wfile = fopen(wfname, "w")) == NULL){
			close(fileno(rfile));
			printf("writing wrognned\n");
			break;
		}
		item.infd = fileno(rfile);
		item.outfd = fileno(wfile);
		item.filename = entry->d_name;
		if(err = putitem(item))
			break;
	}
	err = setdone();
	if(err != ECANCELED)
		seterror(err);
	return NULL;
}
int initproducer(pthread_t *tproducer, void *args){
	int error;
	error = pthread_create(tproducer, NULL, producer, args);
	return (seterror(error));
}
