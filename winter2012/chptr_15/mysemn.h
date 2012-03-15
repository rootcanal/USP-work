typedef FILE mysem_t;

mysem_t *mysem_open(const char *name, int oflag, mode_t mode, unsigned int value);
int mysem_close(mysem_t *sem);
int mysem_unlink(const char *name);
int mysem_wait(mysem_t *sem);
int mysem_post(mysem_t *sem);
