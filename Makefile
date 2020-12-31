CFLAGS?= -pedantic -Wall -Os
libs= X11 lua5.3

12wm: main.o lua.o lua_point.o lua_window.o
	$(CC) $(CFLAGS) $(libs:%=-l%) $^ -o $@

main.o: lua.h

lua.o: luahelp.h

lua_point.o: luahelp.h

lua_window.o: luahelp.h

clean:
	$(RM) 12wm *.o
