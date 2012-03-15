#define MAX_PATH 234
typedef struct pr_struct{
	int owner;
	char filename[MAX_PATH];
	struct pr_struct *next;
} prcmd_t;


int add(prcmd_t *node);
int rremove(prcmd_t **node);
int getnumber(void);
