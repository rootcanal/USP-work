#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <dirent.h>

typedef int (*PATH_FUN)(char *);
static long long total = -1;

/* these sizes are not working at all*/
/* also, if the trailing slash of the pathname isn't sent from stdin, then it craps. the fix is simple to sanitize input, but I've gotta move on to stay on schedule for the quarter*/

int sizepathfun(char *path){
	struct stat sbuf;
	if(stat(path, &sbuf)){
		fprintf(stdout, "%lld %s\n",(long long)sbuf.st_size,path);
		return (long long)sbuf.st_size;
	}
	return -1;
}

int depthfirstapply(char *path, PATH_FUN pathfun){

	struct dirent *direntp;
	char *next;
	long long ret;
	DIR * dirp;
	if((dirp = opendir(path))){
		while((direntp = readdir(dirp)) != NULL){
			next = malloc(sizeof(path)+sizeof(direntp->d_name)+2);
			sprintf(next, "%s%s",path,direntp->d_name);
			if((ret = pathfun(next)) != -1){
				total = total + ret;
			}
			if(direntp->d_type == DT_DIR){
				if(strcmp(direntp->d_name,"..")){
					if(strcmp(direntp->d_name,"."))
						strcat(next, "/");
				}
				depthfirstapply(next, sizepathfun);
			}
			free(next);
		}
	}
	return total;
}

int breadthfirstapply(char *dir, PATH_FUN pathfun){
	struct dirent *direntp;
	long long ret;
	DIR * dirp;
	int l;

	TAILQ_HEAD(tailhead, entry) head;
	struct entry{
		char *path;
		TAILQ_ENTRY(entry) entries;
	} *np1, *np2 = malloc(sizeof(struct entry)), *node = malloc(sizeof(struct entry));

	TAILQ_INIT(&head);
	node->path = dir;
	TAILQ_INSERT_TAIL(&head, node, entries);
	np2->path = malloc(80);
	for(np1 = head.tqh_first; np1 != NULL; np1 = np1->entries.tqe_next){
		if((ret = sizepathfun(dir)) != -1)
			total = total + ret;;
		if((dirp = opendir(dir))){
			while((direntp = readdir(dirp)) != NULL){
				if((l = sizeof(direntp->d_name)+sizeof(np1->path)) >= sizeof(np2->path))
					np2->path = realloc(np2->path, sizeof(l)*2);
				sprintf(np2->path, "%s%s", np1->path, direntp->d_name);
				if(direntp->d_type == DT_DIR){
					if(strcmp(direntp->d_name,"..")){
						if(strcmp(direntp->d_name,"."))
							strcat(np2->path, "/");
					}
				}
				TAILQ_INSERT_TAIL(&head, np2, entries);
			}
		}
		TAILQ_REMOVE(&head, head.tqh_first, entries);
	}
	return ret;
}

int main(int argc, char **argv){
	long long a;
	if(argc == 2){
		if((a = depthfirstapply(argv[1], sizepathfun)) == -1)
			return -1;
	}
	fprintf(stdout, "total %lld\n",a);
	return 0;
}
