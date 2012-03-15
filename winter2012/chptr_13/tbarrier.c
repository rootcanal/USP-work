#include <errno.h>
#include <pthread.h>

#define STARTSIZE 10

typedef struct barrier_s {
	static pthread_cond_t bcond = PTHREAD_COND_INITIALIZER;
	static pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;
	static int count = 0;
	static int limit = 0;
} bar_struct;

static bar_struct *barray = malloc(10*sizeof(struct bar_struct));

int initbarrier(int n, int index) {              /* initialize the barrier to be size n */
   int error;

   if (error = pthread_mutex_lock(&barray[index].bmutex))        /* couldn't lock, give up */
      return error;
   if (limit != 0) {                 /* barrier can only be initialized once */
      pthread_mutex_unlock(&barray[index].bmutex);
      return EINVAL;
   }
   limit = n;
   return pthread_mutex_unlock(&bmutex);
}

int waitbarrier(int index) {    /* wait at the barrier until all n threads arrive */
   int berror = 0;
   int error;

   if (error = pthread_mutex_lock(&barray[index].bmutex))        /* couldn't lock, give up */
      return error;
   if (barray[index].limit <=  0) {                       /* make sure barrier initialized */
      pthread_mutex_unlock(&barray[index].bmutex);
      return EINVAL;
   }
   barray[index].count++;
   while ((barray[index].count < barray[index].limit) && !berror)
      berror =  pthread_cond_wait(&barray[index].bcond, &barray[index].bmutex);
   if (!berror)
      berror = pthread_cond_broadcast(&barray[index].bcond);           /* wake up everyone */
   error = pthread_mutex_unlock(&barray[index].bmutex);
   if (berror)
      return berror;
   return error;
}
