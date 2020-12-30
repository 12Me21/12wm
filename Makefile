CFLAGS?= -pedantic -Wall -Os
libs= X11 lua5.3

12wm: main.o lua.o
	$(CC) $(CFLAGS) $(libs:%=-l%) $^ -o $@

main.o: lua.h

clean:
	$(RM) 12wm *.o
