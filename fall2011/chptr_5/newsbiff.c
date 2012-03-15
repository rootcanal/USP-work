#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>

time_t lastmod(char *pathname){
	struct stat sbuf;
	if(stat(pathname, &sbuf) == -1)
		return -1;
	return sbuf.st_mtime;
}

char *convertnews(char *newsgroup){
	char *newsdir, *tmp, *news;
	if(getenv("NEWSDIR"))
		newsdir = getenv("NEWSDIR");
	else
		newsdir = "/var/spool/news/";

	if((news = malloc(sizeof(newsdir)+sizeof(newsgroup)+1)) != NULL)
		return NULL;
	sprintf(news,"%s%s",newsdir,newsgroup);

	tmp = news;
	while(*tmp){
		if(*tmp == '.')
			*tmp = '/';
		++tmp;
	}
	return news;
}

int main(int argc, char **argv){
	time_t a, b;
	int s = 80, sptr = 80, c, i = 0, j = 0, k;
	char **files = malloc(80), *file = malloc(80);
	FILE *fd;
	if(argc == 3)
		fprintf(stderr, "Usage: %s newsfile sleeptime\n",argv[0]);
	if((fd = fopen(argv[1], "r")) == NULL){
		perror("fopen");
		return -1;
	}
	if(setvbuf(fd, NULL, _IOLBF, 4096)){
		perror("setvbuf");
		return -1;
	}
	while((c = fgetc(fd)) != EOF){
		if(i == sptr){
			sptr = sptr *2;
			files = realloc(files, sptr);
		}
		if(j == s){
			s = s * 2;
			file = realloc(file, s);
		}
		file[j] = c;
		if(c == '\n'){
			file[j] = '\0';
			j = 0;
			i++;
		}
		j++;
	}
	while(1){
		sleep(atoi(argv[2]));
		k = i;
		while(k--){
			if((a = lastmod(argv[1]) == -1)){
				perror("lastmod:");
				return -1;
			}
			if(a != b){
				b = a;
				fprintf(stdout, "%s %s", argv[1],ctime(&a));
			}
		}
	}
	return 0;
}
