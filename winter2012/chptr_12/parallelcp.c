#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "copy_struct.h"
#include "../../fall2011/chapter05/isdirectory.c"
#include "../chapter12/copyfilepass.c"

#define MAXNAME 80
#define R_FLAGS O_RDONLY
#define W_FLAGS (O_WRONLY | O_CREAT)
#define W_PERMS (S_IRUSR | S_IWUSR)

typedef struct copy_struct{
	char *namestring;
	int sourcefd;
	int destinationfd;
	int bytescopied;
	pthread_t tid;
	struct copy_struct *next;
} copyinfo_t;
copyinfo_t *head = NULL;
copyinfo_t *tail = NULL;

void *copyfilepass(void *arg);
int isdirectory(char *path);

void *copydirectory(void *arg){
	copyinfo_t *cp2, *copy = malloc(sizeof(copyinfo_t));
	DIR *dirp;
	struct dirent *direntp;

	char *arga = (char *)arg, **a;
	char *argb = arg+strlen(arga)+2;
	char dest[MAXNAME];
	char src[MAXNAME];

	int error, *bytesp, totalbytes;

	if((dirp = opendir(arga)) == NULL )
		perror("failed to open directory");

	if(!isdirectory(argb))
		mkdir(args[1], 0777);

	head = copy;
	tail = copy;

	while ((direntp = readdir(dirp)) != NULL) {

		copy->next = malloc(sizeof(copyinfo_t));
		copy = copy->next;
		copy->next = NULL;
		tail = copy;

		copy->tid = pthread_self();
		sprintf(src, "%s/%s", arga, direntp->d_name);
		sprintf(dest, "%s/%s", argb, direntp->d_name);
		if(isdirectory(direntp->d_name)){
			sprintf(&a, "%s\0%s", src, dest);
			copydirectory(a);
			continue;
		}
		if ((copy->sourcefd = open(src, R_FLAGS)) == -1){
			fprintf(stderr, "failed to open source file %s\n", direntp->d_name);
			continue;
		}
		if((copy->destinationfd = open(dest, W_FLAGS, W_PERMS)) == -1) {
			fprintf(stderr, "failed to open dest file %s\n", dest);
			continue;
		}
		if (error = pthread_create(copy->tid, NULL, copyfilepass, &copy))
			fprintf(stderr, "failed to create thread %d\n", copy->tid);

		while ((closedir(dirp) == -1) && (errno == EINTR)) ;
	}
	copy = head;
	while(copy != NULL){
		if (error = pthread_join(copy->tid, (void **)&bytesp)){
			fprintf(stderr, "failed to join thread %d\n", copy->tid);
			continue;
		}
		if(close(copy->sourcefd))
			fprintf(stderr, "failed to close src for thread %d\n",copy->tid);
		if(close(copy->destinationfd))
			fprintf(stderr, "failed to close dest for thread %d\n",copy->tid);
		if(bytesp == NULL){
			fprintf(stderr, "thread %d failed to return status\n", copy->tid);
			continue;
		}
		printf("Thread %d copied %d bytes from %s to %s\n", copy->tid, *bytesp, arga, argb);
		totalbytes += *bytesp;
		copy = copy->next;
	}
	printf("Total bytes copied = %d\n", totalbytes);

}

int main(int argc, char *argv[]) {
	pthread_t tid;
	char * wat = &argv + strlen(argv[0]) + 2;

	if(argc < 3){
		fprintf(stderr, "usage: <prog> srcfldr destfldr\n");
		exit(1);
	}
	if((pthread_create(tid, NULL, copydirectory, &wat)))
		fprintf(stderr, "failed to thread wat\n");
	return 1;
}
