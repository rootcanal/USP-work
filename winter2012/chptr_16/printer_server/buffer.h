#define BUFSIZE 8
#define MAXNAME_SIZE 256
typedef struct {
	int infd;
	int outfd;
	char *filename;
} buffer_t;
int getitem(buffer_t *itemp);
int putitem(buffer_t item);

typedef struct pr_struct{
	int owner;
	char filename[PATH_MAX];
	struct pr_struct *nextprcmd;
} prcmd_t;


