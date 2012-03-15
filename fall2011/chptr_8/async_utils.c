#include <aio.h>
#include <errno.h>
#include <signal.h>
#include "restart.h"

static struct aiocb aiocb_read, aiocb_write;
static sig_atomic_t doneflag;
static int fdout, globalerror, totalbytes;
static int readstart();
static void seterror(int error);

/* ARGSUSED */
static void aiohandler(int signo, siginfo_t *info, void *context) {
	int  myerrno, mystatus, serrno;
	serrno = errno;
	myerrno = aio_error(&aiocb_read);
	if (myerrno == EINPROGRESS) {
		errno = serrno;
		return;
		}
	if (myerrno) {
		seterror(myerrno);
		errno = serrno;
		return;
	}
	mystatus = aio_return(&aiocb_read);
	totalbytes += mystatus;
	aiocb_read.aio_offset += mystatus;
	aiocb_write.aio_nbytes = mystatus;
	if (mystatus == 0)
		doneflag = 1;
	else if (aio_write(&aiocb_write) == -1)
		seterror(errno);
	else if (readstart() == -1)
		seterror(errno);
	errno = serrno;
}

static int readstart() {                     /* start an asynchronous read */
	int error;
	if (error = aio_read(&aiocb_read))
		seterror(errno);
	return error;
}

static void seterror(int error) {            /* update globalerror if zero */
	if (!globalerror)
		globalerror = error;
	doneflag = 1;
}

/* --------------------------Public Functions ---------------------------- */
int getbytes() {
	if (doneflag)
		return totalbytes;
	errno = EINVAL;
	return -1;
}

int getdone() {                                          /* check for done */
	return doneflag;
}

int geterror() {               /* return the globalerror value if doneflag */
	if (doneflag)
		return globalerror;
	errno = EINVAL;
	return errno;
}

int initread(int fdread, int fdwrite, int signo, char *buf, int bufsize) {
	int error;
	sigset_t oldset, fullset;

	if ((sigfillset(&fullset) == -1) ||
	 (sigprocmask(SIG_SETMASK, &fullset, &oldset) == -1)) {
		seterror(errno);
		return -1;
	}
	aiocb_read.aio_fildes = fdread;                          /* set up structure */
	aiocb_read.aio_offset = 0;
	aiocb_read.aio_buf = (void *)buf;
	aiocb_read.aio_nbytes = bufsize;
	aiocb_read.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	aiocb_read.aio_sigevent.sigev_signo = signo;
	aiocb_read.aio_sigevent.sigev_value.sival_ptr = &aiocb_read;
	fdout = fdwrite;

	aiocb_write.aio_fildes = fdwrite;
	aiocb_write.aio_offset = 0;
	aiocb_write.aio_buf = (void *)buf;
	aiocb_write.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	aiocb_write.aio_sigevent.sigev_signo = signo;
	aiocb_write.aio_sigevent.sigev_value.sival_ptr = &aiocb_write;


	doneflag = 0;
	globalerror = 0;
	totalbytes = 0;
	error = readstart();                                /* start first read */
	if (sigprocmask(SIG_SETMASK, &oldset, NULL) == -1) {
		seterror(errno);
		return -1;
	}
	return error;
}

int initsignal(int signo) {        /* set up the handler for the async I/O */
	struct sigaction newact;

	newact.sa_sigaction = aiohandler;
	newact.sa_flags = SA_SIGINFO;
	if ((sigemptyset(&newact.sa_mask) == -1) ||
	 (sigaction(signo, &newact, NULL) == -1))
		return -1;
	return 0;
}

int suspenduntilmaybeready() {            /* return 1 if done, 0 otherwise */
	const struct aiocb *aiocblist;
	aiocblist = &aiocb_read;
	aio_suspend(&aiocblist, 1, NULL);
	return doneflag;
}
