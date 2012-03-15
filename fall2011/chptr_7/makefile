CC = gcc
COPS =
LINT = splint
LOPS = -warnposixheaders

all: ring ring1 ring2 ring3 ring4 ring5

ring: ring.c
	$(CC) $(COPS) -o ring ring.c

ring1: ring1.c
	$(CC) $(COPS) -o ring1 ring1.c

ring2: ring2.c
	$(CC) $(COPS) -o ring2 ring2.c

ring3: ring3.c
	$(CC) $(COPS) -o ring3 ring3.c

ring4: ring4.c
	$(CC) $(COPS) -o ring4 ring4.c

ring5: ring5.c
	$(CC) $(COPS) -o ring5 ring5.c

lintall: lintring lintring1 lintring2 lintring3 lintring4 lintring5

lintring:
	$(LINT) $(LOPS) ring.c

lintring1:
	$(LINT) $(LOPS) ring1.c

lintring2:
	$(LINT) $(LOPS) ring2.c

lintring3:
	$(LINT) $(LOPS) ring3.c

lintring4:
	$(LINT) $(LOPS) ring4.c

lintring4:
	$(LINT) $(LOPS) ring4.c

clean:
	rm -f *.o core ring ring1 ring2 ring3 ring4 ring5
