typedef struct copy_struct{
	char *namestring;
	int sourcefd;
	int destinationfd;
	int bytescopied;
	pthread_t tid;
	struct copy_struct *next;
} copyinfo_t;
copyinfo_t *head = NULL;
copyinfo_t *tail = NULL;


