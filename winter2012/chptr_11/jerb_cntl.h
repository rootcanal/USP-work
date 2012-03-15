typedef enum jstatus
		{FOREGROUND, BACKGROUND, STOPPED, DONE, TERMINATED}
	job_status_t;

typedef struct job_struct{
	char *cmdstring;
	pid_t pgid;
	int job;
	job_status_t jobstat;
	struct job_struct *next;
} joblist_t;


