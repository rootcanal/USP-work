#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "show.h"
#define MAXTIMERS 5


typedef struct timerdata_t {
	long active[MAXTIMERS];
	int events[MAXTIMERS];
	int numevents;
	int running;
} timerdata_t;

static timerdata_t timer;

static void remove_event(int i){
	while(i < timer.numevents - 1)
		timer.events[i] = timer.events[++i];
	timer.numevents--;
}

static void timerhandler(int signo){
	timer.events[timer.numevents++] = timer.running;
	timer.active[timer.running] = -1;
	timer.running = -1;
}

/* ============================== public functions ========================*/
int getevent(int eventnumber){
	if(eventnumber < 0 || eventnumber >= timer.numevents)
		return -1;
	return timer.events[eventnumber];
}

int getnumevents(){
	return timer.numevents;
}

int getrunning(){
	return timer.running;
}

long getvalue(int n){
	if(n >= MAXTIMERS)
		return -1;
	return  timer.active[n];
}

int removetop(){
	int i, ret = timer.events[0];
	for(i = 0; i < timer.numevents-2; i++)
		timer.events[i] = timer.events[i+1];
	timer.events[timer.numevents-1] = NULL;
	timer.active[ret] = -1;
	timer.numevents--;
	return ret;
}

int timerinit(){
	int i;
	timer.running = -1;
	timer.numevents = 0;
	for(i = 0; i < MAXTIMERS; i++){
		timer.active[i] = -1;
		timer.events[i] = NULL;
	}
	if(catchsetup(timerhandler))
		return -1;
	showinit(MAXTIMERS);
	return 0;
}

void timerstart(int n, long interval){
	int i;
	for(i = 0; i < timer.numevents; i++){
		if(timer.events[i] == n){
			remove_event(i);
			break;
		}
	}
	timer.running = n;
	timer.active[n] = (int)interval;
	sethardwaretimer(interval);
}

void timerstop(int n){
	int i;
	timer.active[n] = -1;
	for(i = 0; i < timer.numevents; i++){
		if(timer.events[i] == n){
			remove_event(i);
			break;
		}
	}
	timer.active[n] = -1;
}

void waitforevent(){
	waitforinterrupt();
}
