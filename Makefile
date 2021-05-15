CC= gcc
CFLAGS= -Os -std=c89

all:
	$(CC) $(CFLAGS) main.c -o ctok -lm -g

clean:
	rm -f *.exe *.out *.bin *.o
