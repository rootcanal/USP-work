#include <errno.h>
#include "prcmd.h"
#include <pthread.h>

static prcmd_t *prhead = NULL;
static prcmd_t *prtail = NULL;
static int pending = 0;
static pthread_mutex_t prmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t items = PTHREAD_COND_INITIALIZER;

int add(prcmd_t *node){
	int error;
	if(error = pthread_mutex_lock(&prmutex))
		return error;
	pending++;
	if(prtail != NULL)
		prtail->next = node;
	prtail = node;
	if(prhead == NULL){
		prhead = node;
		prhead->next = prtail;
	}
	if(error = pthread_cond_signal(&items)){
		pthread_mutex_unlock(&prmutex);
		return error;
	}
	return pthread_mutex_unlock(&prmutex);
}
int rremove(prcmd_t **node){
	int error;
	if(error = pthread_mutex_lock(&prmutex))
		return error;
	while(pending <= 0 && !error)
		error = pthread_cond_wait(&items, &prmutex);
	if(error){
		pthread_mutex_unlock(&prmutex);
		return error;
	}
	*node = prhead;
	prhead = prhead->next;
	pending--;
	return mutex_unlock(&prmutex);
}
int getnumber(void){
	return pending;
}
