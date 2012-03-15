#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

int checkexecutable(char *name){
	int euid = geteuid();
	int egid = getegid();
	struct stat sbuf;

	if(stat(name, &sbuf) == -1){
		return false;
	}
	if((sbuf.st_mode & S_IXOTH) == S_IXOTH)
		return true;
	if((sbuf.st_mode & S_IXGRP) == S_IXGRP && sbuf.st_gid == egid)
		return true;
	if((sbuf.st_mode & S_IXUSR) == S_IXUSR && sbuf.st_uid == euid)
		return true;

	return false;
}

int main(int argc, char **argv){
	int a = 1, b = 0;
	size_t l;
	char *path, *p, *pp;
	struct stat sbuf;
	struct dirent *direntp;
	DIR *dirp;
	if(argc == 1)
		return -1;
	if(!strcmp(argv[a], "-a")){
		a++;
		b++;
	}



	if((path = getenv("PATH")) == NULL){
		perror("no path defined");
		return -1;
	}
	l = strlen(p)+1;
	p = malloc(sizeof(char)*strlen(path)*2);
	pp = malloc(sizeof(char)*strlen(path)*2);
	p = strtok(path, ":");
	while((p != NULL)){
		if((!stat(p, &sbuf)) && (S_ISDIR(sbuf.st_mode))){
			if((dirp = opendir(p)) == NULL)
				return -1;
			while((direntp = readdir(dirp)) != NULL){
				if(strcmp(direntp->d_name, argv[a]) == 0){
					if((pp = realloc(pp,sizeof(direntp->d_name)+sizeof(p)*2)) == NULL)
						return -1;
					sprintf(pp, "%s/%s",p,direntp->d_name);
					if(checkexecutable(pp)){
						fprintf(stdout, "%s\n",pp);
						if(b)
							break;
						return 0;
					}
				}
			}
		}
		p = strtok(NULL, ":");
	}
	return 0;
}
