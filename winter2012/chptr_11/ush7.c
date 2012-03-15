#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "jerb_cntl.h"
#define BACK_SYMBOL '&'
#define PROMPT_STRING "ush7>>"
#define QUIT_STRING "q"

void executecmd(char *incmd);
int signalsetup(struct sigaction *def, sigset_t *mask, void (*handler)(int));
static sigjmp_buf jumptoprompt;
static volatile sig_atomic_t okaytojump = 0;

/* ARGSUSED */
static void jumphd(int signalnum) {
	if (!okaytojump) return;
	okaytojump = 0;
	siglongjmp(jumptoprompt, 1);
}

int main (void) {
	char *backp;
	sigset_t blockmask;
	pid_t childpid;
	struct sigaction defhandler;
	int inbackground;
	char inbuf[MAX_CANON];
	int len, jerbno;
	job_status_t s = BACKGROUND;

	if (signalsetup(&defhandler, &blockmask, jumphd) == -1) {
		perror("Failed to set up shell signal handling");
		return 1;
	}

	for( ; ; ) {
		if ((sigsetjmp(jumptoprompt, 1)) &&	/* if return from signal, newline */
			 (fputs("\n", stdout) == EOF) )
			continue;
		okaytojump = 1;
		printf("%d",(int)getpid());
		if (fputs(PROMPT_STRING, stdout) == EOF)
			 continue;
		if (fgets(inbuf, MAX_CANON, stdin) == NULL)
			 continue;
		len = strlen(inbuf);
		if (inbuf[len - 1] == '\n')
			 inbuf[len - 1] = 0;
		if (strcmp(inbuf, QUIT_STRING) == 0)
			 break;
		if (strncmp(inbuf, "jobs", 4) == 0){
			showjobs();
			continue;
		}
		if ((backp = strchr(inbuf, BACK_SYMBOL)) == NULL)
			 inbackground = 0;
		else {
			 inbackground = 1;
			 *backp = 0;
		}
		if (sigprocmask(SIG_BLOCK, &blockmask, NULL) == -1)
			perror("Failed to block signals");
		if ((childpid = fork()) == -1)
			perror("Failed to fork");
		if(inbackground && childpid){
			if((jerbno = add(childpid, inbuf, BACKGROUND)) == -1)
				perror("failed to add jerb");
			fprintf(stderr, "[%d] ",jerbno);
		}
		if (childpid == 0) {
			if (inbackground){
				if((setpgid(0, 0) == -1))
					return 1;
				fprintf(stderr, "%d ",getpid());
			}
			if ((sigaction(SIGINT, &defhandler, NULL) == -1) ||
				 (sigaction(SIGQUIT, &defhandler, NULL) == -1) ||
				 (sigprocmask(SIG_UNBLOCK, &blockmask, NULL) == -1)) {
				 perror("Failed to set signal handling for command ");
				 return 1;
			}
			executecmd(inbuf);
			return 1;
		}
		if (sigprocmask(SIG_UNBLOCK, &blockmask, NULL) == -1)
			perror("Failed to unblock signals");
		if (!inbackground)		  /* wait explicitly for the foreground process */
			 waitpid(childpid, NULL, 0);
		while (waitpid(-1, NULL, WUNTRACED) > 0);	 /* wait for background procs */
	}
	return 0;
}
