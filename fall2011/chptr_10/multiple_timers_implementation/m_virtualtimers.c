#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "m_show.h"
#define MAXTIMERS 5


typedef struct timerdata_t {
	long active[MAXTIMERS];
	int events[MAXTIMERS];
	int numevents;
	int running;
} timerdata_t;

static timerdata_t timer;

static int traceflag = 1;

static void check_event(int n){
	int i;
	for(i = 0; i < timer.numevents; i++){
		if(timer.events[i] == n){
			while(i < timer.numevents -1)
				timer.events[i] = timer.events[++i];
			timer.numevents--;
		}
	}
}

static void timerhandler(int signo){
	show(traceflag, "Timer Handler Enter", timer.running, -1, -1);
	int t_next_val = INT_MAX, i, t_next_index = -1;
	timer.events[timer.numevents++] = timer.running;
	timer.active[timer.running] = -1;
	for(i = 0; i < MAXTIMERS; i++){
		if(timer.active[i] != -1 && timer.active[i] < t_next_val){
			t_next_index = i;
			t_next_val = timer.active[i];
		}
	}
	for(i = 0; i < MAXTIMERS; i++){
		if(timer.active[i] != -1 && i != t_next_index)
			timer.active[i] = timer.active[i] - t_next_val;
	}
	timerstart(t_next_index, t_next_val);
	show(traceflag, "Timer Handler Exit", timer.running, -1, -1);
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
	show(traceflag, "Timer Start Enter", n, interval, 0);
	long t_cur, t_start, t_left;
	int i;
	if(running == -1){
		check_event(n);
		timer.running = n;
		timer.active[n] = (int)interval;
		sethardwaretimer(interval);
	}
	else {
		if((t_cur = gethardwaretimer()) < interval) {
			t_start = interval - t_cur;
			t_left = interval + t_start;
			timer.active[n] = t_left;
		}
		else {
			timer.running = n;
			timer.active[n] = (int)interval;
			sethardwaretimer(interval);
			for(i = 0; i < MAXTIMERS; i++)
				timer.active[i] = timer.active[i] - interval;
		}
	}
	show(traceflag, "Timer Start Exit", n, interval, 0);
}

void timerstop(int n){
	timer.active[n] = -1;
	check_event(n);
	timer.active[n] = -1;
}

void waitforevent(){
	waitforinterrupt();
}
