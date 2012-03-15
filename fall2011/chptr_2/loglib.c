#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

typedef struct list_struct {
	data_t item;
	struct list_struct *next;
} log_t;

static log_t *headptr = NULL;
static log_t *tailptr = NULL;

int addmsg(data_t data) {
	log_t *new_log;
	int log_size;

	log_size = sizeof(log_t) + strlen(data.string) + 1;
	if((new_log = (log_t *)(malloc(log_size))) == NULL)
		return -1;
	new_log->item.time = data.time;
	new_log->item.string = (char *)new_log + sizeof(log_t);
	strcpy(new_log->item.string, data.string);
	new_log->next = NULL;
	if(headptr == NULL)
		headptr = new_log;
	else
		tailptr->next = new_log;
	tailptr = new_log;
	return 0;
}

void clearlog(void){
	log_t *next = headptr;
	while(next){
		log_t *ptr = next;
		next = next->next;
		free(ptr->item.string);
		free(ptr);
	}
}

char *getlog(void) {
	log_t * next = headptr;
	char *a = next->item.string;
	while(next){
		next = next->next;
		char *b = next->item.string;
		char *c = a;
		if((a = malloc(sizeof(strlen(c) + strlen(b)) + 2)) == NULL)
			return NULL;
		sprintf(a, "%s%s", b, c);
	}
	return a;
}

int savelog(char *filename) {
	log_t *ptr = headptr;
	FILE * file;

	if((file = fopen(filename, "a")) == NULL)
		return -1;
	while(ptr != NULL){
		if((fprintf(file,"%s\n", ptr->item.string)) < 0)
			return -1;
		ptr = ptr->next;
	}
	return 0;
}
