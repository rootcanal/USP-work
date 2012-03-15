#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINE 80

#ifndef MAXTIMERS
#define MAXTIMERS 5
#endif


int main(int argc, char **argv){
	FILE *fp;
	char *line = malloc(sizeof(MAXLINE) +1), *ival, *cmd, cmds[MAXTIMERS][MAXLINE];
	long  *intervals[MAXTIMERS], interval, t_num = 0;

	if(argc != 2){
		fprintf(stderr, "Usage: mycron <file>\n");
		exit(1);
	}
	if((fp = fopen(argv[1], 'r')) == NULL)
		perror("wat file not work");

	if(timerinit() == -1)
		perror("timeriniting didn't go so well");

	while(fgets(line, MAXLINE, fp) != NULL){
		ival = strtok(line, " ");
		cmd = strtok(NULL, " ");
		interval = atoi(ival);
		intervals[t_num] = interval;
		cmds[t_num] = cmd;
		timerstart(t_num++, interval);
	}
	for(;;){
		waitforevent();
		t_num = removetop();
		timerstart(t_num, intervals[t_num]);
		system(cmds[t_num]);
	}
	return 0;
}
