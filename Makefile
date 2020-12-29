CFLAGS?=-Os -pedantic -Wall

12wm: main.o
	$(CC) $(CFLAGS) -lX11 $^ -o $@

clean:
	$(RM) 12wm *.o
