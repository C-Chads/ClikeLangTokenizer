CC= gcc
CFLAGS= -Os -std=c89

all:
	$(CC) $(CFLAGS) main.c -o a.out -lm -g

clean:
	rm -f *.exe *.out *.bin *.o
