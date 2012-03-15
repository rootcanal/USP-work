#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

#define MAILDIR "/var/mail/"
#ifndef MAILCHECK
#define MAILCHECK 600
#endif

int main(int argc, char **argv) {
	struct stat sbuf;
	int b = MAILCHECK, opt;
	struct passwd *pwd = getpwuid(getuid());
	char *usr = pwd->pw_name, *mailpath;

	if(argc == 1){
		if(!getenv("MAIL")){
			mailpath = malloc(sizeof(MAILDIR)+sizeof(usr)+1);
			sprintf(mailpath, "%s%s", MAILDIR, usr);
		}else{
			mailpath = getenv("MAIL");
		}
	}
	while((opt = getopt(argc, argv, "s:p:")) != -1){
		switch(opt){
			case 's':
				b = atoi(optarg);
				break;
			case 'p':
				mailpath = optarg;
				break;
			default: /* '?' */
				fprintf(stderr, "Usage %s [-s n] [-p pathname]", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

   for( ; ; ) {
      if ((stat(mailpath, &sbuf)) != -1 && sbuf.st_size) {
         fprintf(stderr, "%s", "\007");
      }
      sleep(b);
   }
}
