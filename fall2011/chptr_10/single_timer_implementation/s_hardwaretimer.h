int blockinterrupt(void);
long gethardwaretimer(void);
int isinterruptblocked(void);
void sethardwaretimer(long interval);
int catchsetup(void (*handler)(void));
void stophardwaretimer(void);
void unblockinterrupt(void);
void waitforinterrupt(void);
