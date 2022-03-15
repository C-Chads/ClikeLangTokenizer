CC= gcc
CFLAGS= -Os -std=c89

ctok:
	$(CC) $(CFLAGS) main.c -o ctok -lm -g

install: ctok
	cp ./ctok /usr/local/bin/

uninstall:
	rm /usr/local/bin/ctok

clean:
	rm -f *.exe *.out *.bin *.o ctok
