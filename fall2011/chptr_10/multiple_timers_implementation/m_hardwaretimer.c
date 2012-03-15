#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

struct itimerval t;
static int isblocked = 0;

int blockinterrupt(){
	sigset_t newsigset, oldsigset;
	if((sigemptyset(&newsigset) == -1) ||
	 (sigaddset(&newsigset, SIGALRM) == -1))
		perror(" failed to initialize at blockinterrupt() in hardwaretimer.c: ");
	else if(sigprocmask(SIG_BLOCK, &newsigset, &oldsigset) == -1)
		perror("failed to block at blockinterrupt() in hardwaretimer.c: ");
	isblocked = 1;
	if(sigismember(&oldsigset, SIGALRM))
		return 1;
	return 0;
}

int catchsetup(void (*handler)(int)){
	struct sigaction act;
	act.sa_handler = handler;
	act.sa_flags = 0;
	if((sigemptyset(&act.sa_mask) == -1) ||
	 (sigaction(SIGALRM, &act, NULL) == -1)){
		perror("Failed to set SIGALRM in catchsetup() of hardwaretimer.c");
		return -1;
	}
	return 0;
}

long gethardwaretimer(){
	if(getitimer(ITIMER_REAL, &t) == -1){
		perror("failed to getharwaretimer() at hardwaretimer.c");
		return -1;
	}
	return t.it_value.tv_usec;
}

int isinterruptblocked(){
	return isblocked;
}

void sethardwaretimer(long interval){
	t.it_value.tv_sec = interval;
	t.it_value.tv_usec = interval;
	setitimer(ITIMER_REAL, &t, NULL);
}

/* the book says this one is hard and will be gone over later;
	I'll wait for them to explain further */
void stophardwaretimer(){
	;
}

void unblockinterrupt(){
	sigset_t newsigset, oldsigset;
	if((sigemptyset(&newsigset) == -1) ||
	 (sigaddset(&newsigset, SIGALRM) == -1))
		perror(" failed to initialize at unblockinterrupt() in hardwaretimer.c: ");
	else if(sigprocmask(SIG_UNBLOCK, &newsigset, &oldsigset) == -1)
		perror("failed to unblock at unblockinterrupt() in hardwaretimer.c: ");
	isblocked = 0;

}

void waitforinterrupt(){
	fprintf(stderr, "wat did sumthin in wat waitedforinterrupt\n");
	sigset_t maskall, maskmost, maskold;
	if((sigfillset(&maskall) == -1) ||
	 (sigfillset(&maskmost) == -1) ||
	 (sigfillset(&maskold) == -1) ||
	 (sigdelset(&maskmost, SIGALRM) == -1))
		perror("failed to  waitforinterrupt() in hardwaretimer.c: ");
	if(sigprocmask(SIG_SETMASK, &maskall, &maskold) == -1)
		perror("failed to sigprocmask in waitforinterrupt");
	sigsuspend(&maskmost);
	if(sigprocmask(SIG_SETMASK, &maskold, NULL) == -1)
		perror("failed to sigprocmask in waitforinterrupt");
}
