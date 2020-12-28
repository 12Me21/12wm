PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall

12wm: main.o
	$(CC) $(CFLAGS) -L$(PREFIX)/lib -lX11 $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) -I$(PREFIX)/include -L$(PREFIX)/lib -lX11 -c $^ -o $@

clean:
	$(RM) 12wm
	$(RM) *.o

