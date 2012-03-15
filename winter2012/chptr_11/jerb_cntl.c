#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jerb_cntl.h"

static joblist_t *jobhead = NULL;
static joblist_t *jobtail = NULL;

int add(pid_t pgid, char *cmd, job_status_t status){
	joblist_t *new = malloc(sizeof(struct job_struct));
	new->cmdstring = cmd;
	new->jobstat = status;
	new->pgid = pgid;
	new->next = NULL;
	if(jobhead == NULL)
		jobhead = new;
	new->job = getlargest() + 1;
	if(jobtail)
		jobtail->next = new;
	jobtail = new;
	return jobtail->job;
}

int delete(int job){
	joblist_t *jerb = jobhead, *tmp;
	while (jerb->job < job && jerb)
		jerb = jerb->next;
	if(!jerb)
		return -1;
	if(jobhead->job == jerb->job)
		jobhead = jobhead->next;
	tmp = jerb;
	while((jerb = jerb->next))
		jerb->job--;
	free(tmp);
	return job;
}

void showjobs(){
	joblist_t *job = jobhead;
	char stat[5][15]= {"FOREGROUND\0", "BACKGROUND\0", "STOPPED\0", "DONE\0", "TERMINATED\0"};
	if(!job)
		fprintf(stderr, "no jobs\n");
	while(job){
		fprintf(stderr, "%d %s %d %s\n", job->job, stat[job->jobstat], job->pgid, job->cmdstring);
		job = job->next;
	}
}

int setstatus(int job, job_status_t status){
	joblist_t *jerb = jobhead;
	while(jerb->job != job && jerb)
		jerb = jerb->next;
	if(jerb)
		jerb->jobstat = status;
	else
		return -1;
	return 0;
}

int getstatus(int job, job_status_t *pstatus){
	joblist_t *jerb = jobhead;
	while(jerb->job != job && jerb)
		jerb = jerb->next;
	if(jerb)
		*pstatus = jerb->jobstat;
	else
		return -1;
	return 0;
}

pid_t getprocess(int job){
	joblist_t *jerb = jobhead;
	while(jerb->job != job && jerb)
		jerb = jerb->next;
	if(jerb)
		return jerb->pgid;
	return 0;
}

int getlargest(){
	joblist_t *jerb = jobhead;
	if(!jerb)
		return 0;
	int i = jerb->job;
	while((jerb = jerb->next))
		i = i > jerb->job ? jerb->job : jerb->job;
	return i;
}
/*
int main(int argc, char **argv){
	int a = add(12, "cmd", FOREGROUND);
	fprintf(stderr, "added a successfully\n");
	int b = add(22345, "cmd2", BACKGROUND);
	fprintf(stderr, "added a n b successfully\n");
	job_status_t *pstat = malloc(sizeof(job_status_t));
	showjobs();
	a = delete(a);
	fprintf(stderr, "deleted a with this return: %d\n",a);
	showjobs();
	if(setstatus(1, FOREGROUND))
		perror("failed to set status for b");
	fprintf(stderr, "set status of b successfully\n");
	if(getstatus(1, pstat))
		perror("failed to get status for b");
	fprintf(stderr, "get status of b successfully\n");
	showjobs();
}
*/
