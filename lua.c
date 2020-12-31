#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <string.h>

#include "luahelp.h"

Display* D;
lua_State* L;

void luaKeyPress(XKeyEvent event) {
	int type = lua_getglobal(L, "key_press");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	Lc_window_create(event.subwindow);
	char* name = XKeysymToString(XkbKeycodeToKeysym(D, event.keycode, 0, 0));
	if (name)
		lua_pushstring(L, name);
	else
		lua_pushnil(L);
	int status = lua_pcall(L, 2, 0, 0);
	if (status) {
		printf("Error in lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaClick(XButtonEvent event) {
	int type = lua_getglobal(L, "left_click");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	Lc_window_create(event.subwindow);
	Lc_point_create(event.x_root, event.y_root);
	int status = lua_pcall(L, 2, 0, 0);
	if (status) {
		printf("Error in luas: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaMoveDone(XKeyEvent start, XKeyEvent end, XWindowAttributes winstart) {
	int type = lua_getglobal(L, "move_done");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	Lc_window_create(start.subwindow);
	Lc_point_create(winstart.x, winstart.y);
	Lc_point_create(start.x_root, start.y_root);
	Lc_point_create(end.x_root, end.y_root);
	int status = lua_pcall(L, 4, 0, 0);
	if (status) {
		printf("Error in lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaMoveDuring(XKeyEvent start, XKeyEvent end, XWindowAttributes winstart) {
	int type = lua_getglobal(L, "move_during");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	Lc_window_create(start.subwindow);
	Lc_point_create(winstart.x, winstart.y);
	Lc_point_create(start.x_root, start.y_root);
	Lc_point_create(end.x_root, end.y_root);
	int status = lua_pcall(L, 4, 0, 0);
	if (status) {
		printf("Error in lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1); //why?
	}
}

void luaMoveStart(XKeyEvent start) {
	int type = lua_getglobal(L, "move_start");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	Lc_window_create(start.subwindow);
	Lc_point_create(start.x_root, start.y_root);
	int status = lua_pcall(L, 2, 0, 0);
	if (status) {
		printf("Error in lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaInit(Display* d) {
	D = d;
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadfile(L, "script.lua");
	lua_pcall(L, 0, LUA_MULTRET, 0);
	
	Lc_point_init();
	Lc_window_init();
}
