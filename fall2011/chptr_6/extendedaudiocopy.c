#include <stdio.h>
#include <stdlib.h>
#include "audiolib.h"

int main (void) {
	int recbufsize = get_record_buffer_size();
	int playbufsize = get_play_buffer_size();
	int bytesread;
   char recbuffer[recbufsize], playbuffer[playbufsize];

   if (open_audio() == -1) {
      perror("Failed to open audio");
      return 1;
   }
	playbufsize = get_play_buffer_size();
	recbufsize = get_record_buffer_size();
   for( ; ; ) {
      if ((bytesread = read_audio(recbuffer, recbufsize)) == -1) {
          perror("Failed to read microphone");
          break;
      } else if (write_audio(recbuffer, playbufsize) == -1) {
          perror("Failed to write to speaker");
          break;
      }
   }
   close_audio();
   return 1;
}
