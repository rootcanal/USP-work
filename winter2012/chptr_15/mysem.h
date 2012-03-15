typedef int mysem_t;

int mysem_init(mysem_t *sem, int pshared, unsigned int value);
int mysem_destroy(mysem_t *sem);
int mysem_post(mysem_t *sem);
