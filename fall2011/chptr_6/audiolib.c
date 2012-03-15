#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stropts.h>
#include <unistd.h>
#include <sys/audio.h>
#include <sys/time.h>
#include "restart.h"
#define AUDIO "/dev/audio"

static int audio_fd = -1;   /* audio device file descriptor */

int open_audio(void) {
   while (((audio_fd = open(AUDIO, O_RDWR)) == -1) && (errno == EINTR)) ;
   if (audio_fd == -1)
      return -1;
   return 0;
}

void close_audio(void) {
   r_close(audio_fd);
   audio_fd = -1;
}

int read_audio(char *buffer, int maxcnt) {
   return r_read(audio_fd, buffer, maxcnt);
}

int write_audio(char *buffer, int maxcnt) {
   return r_write(audio_fd, buffer, maxcnt);
}

int get_record_buffer_size(void) {
   audio_info_t myaudio;
   if (audio_fd == -1)
      return -1;
   if (ioctl(audio_fd, AUDIO_GETINFO, &myaudio) == -1)
      return -1;
   else
      return myaudio.record.buffer_size;
}

int play_file(char *filename){
	int bytesread, byteswritten = -1, fd;
	char buf[1024];
	if(audio_fd == -1)
		return -1;
	if((fd = open(filename, O_RDONLY)) == -1)
		return -1;
	while((bytesread = r_read(fd, buf, 1024)) != -1)
		byteswritten += r_write(audio_fd, buf, bytesread);
	return byteswritten;
}

int record_file(char *filename, int seconds){
	int bytesread, byteswritten = -1, fd;
	time_t t, n;
	char buf[1024];
	if(audio_fd == -1)
		return -1;
	if((fd = open(filename, O_APPEND)) == -1)
		return -1;
	if(time(&t) == -1)
		return -1;
	while((n = time(&n)) != -1 && (n - t) > seconds){
		if ((bytesread = read_audio(buf, 1024)) == -1)
			return -1;
		byteswritten += r_write(fd, buf, bytesread);
	}
	return byteswritten;
}

int get_record_sample_rate(void){
	audio_info_t myaudio;
	if(audio_fd == -1)
		return -1;
	if (ioctl(audio_fd, AUDIO_GETINFO, &myaudio) == -1)
		return -1;
	else
		return myaudio.record.sample_rate;
}

int get_play_buffer_size(void){
	audio_info_t myaudio;
	if(audio_fd == -1)
		return -1;
	if (ioctl(audio_fd, AUDIO_GETINFO, &myaudio) == -1)
		return -1;
	else
		return myaudio.play.buffer_size;
}

int get_play_sample_rate(void){
	audio_info_t myaudio;
	if(audio_fd == -1)
		return -1;
	if (ioctl(audio_fd, AUDIO_GETINFO, &myaudio) == -1)
		return -1;
	else
		return myaudio.play.sample_rate;
}

int set_play_volume(double volume){
	audio_info_t myaudio;
	if(volume < 0 || volume > 1)
		return -1;
	if(audio_fd == -1)
		return -1;
	if (ioctl(audio_fd, AUDIO_GETINFO, &myaudio) == -1)
		return -1;
	else
		myaudio.play.gain = volume;
	return 0;
}

int set_record_volume(double volume){
	audio_info_t myaudio;
	if(volume < 0 || volume > 1)
		return -1;
	if(audio_fd == -1)
		return -1;
	if (ioctl(audio_fd, AUDIO_GETINFO, &myaudio) == -1)
		return -1;
	else
		myaudio.record.gain = volume;
	return 0;
}


