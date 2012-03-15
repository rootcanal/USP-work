#include <unistd.h>
#include "restart.h"

#include "copy_struct.h"

void *copyfilepass(void *arg)  {
   copyinfo_t *copy = (copyinfo_t *)arg;

   copy->bytescopied = copyfile(copy->sourcefd, copy->destinationfd);
   r_close(copy->sourcefd);
   r_close(copy->destinationfd);
   return &copy;
}
