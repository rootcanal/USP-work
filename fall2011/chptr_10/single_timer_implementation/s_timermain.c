#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv){
	int t_num, ival;

	for(;;){
		if(timerinit() == -1)
			perror("watdo teh timerinit()");
		fprintf(stderr, "watdo teh nums: \n");
		scanf("%d %d", &t_num, &ival);
		fprintf(stderr, "lol be startin that timer \n");
		timerstart(t_num, ival);
		waitforevent();
		fprintf(stdout, "lol I waited n stuff\n");

	}
	return 1;
}
