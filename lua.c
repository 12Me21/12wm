#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
#include <string.h>

static Display* D;

static lua_State* L;

static void new_point(int x, int y) {
	int* data = lua_newuserdata(L, sizeof(int)*2);
	data[0] = x;
	data[1] = y;
	luaL_setmetatable(L, "point");
}

static void new_window(Window w) {
	if (!w) {
		lua_pushnil(L);
		return;
	}
	Window* data = lua_newuserdata(L, sizeof(Window)*2);
	data[0] = w;
	luaL_setmetatable(L, "window");
}

static int window_kill(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "wrong number of arguments");
	Window* data = lua_touserdata(L, 1);
	int status = XKillClient(D, data[0]);
	lua_pushboolean(L, status==0);
	return 1;
}

static int window_focus(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "wrong number of arguments");
	Window* data = lua_touserdata(L, 1);
	int status = XSetInputFocus(D, data[0], RevertToPointerRoot, CurrentTime);
	XRaiseWindow(D, data[0]);
	lua_pushboolean(L, status==0);
	return 1;
}

static int window_move(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "wrong number of arguments");
	Window* w = lua_touserdata(L, 1);
	int* p = lua_touserdata(L, 2);
	int status = XMoveWindow(D, w[0], p[0], p[1]);
	lua_pushboolean(L, status==0);
	return 1;
}

void luaKeyPress(XKeyEvent event) {
	int type = lua_getglobal(L, "key_press");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	new_window(event.subwindow);
	char* name = XKeysymToString(XkbKeycodeToKeysym(D, event.keycode, 0, 0));
	if (name)
		lua_pushstring(L, name);
	else
		lua_pushnil(L);
	int status = lua_pcall(L, 2, 0, 0);
	if (status) {
		printf("Error in luaKeyPress: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaClick(XButtonEvent event) {
	int type = lua_getglobal(L, "left_click");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	new_window(event.subwindow);
	new_point(event.x_root, event.y_root);
	int status = lua_pcall(L, 2, 0, 0);
	if (status) {
		printf("Error in luaKeyPress: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaMoveDone(XKeyEvent start, XKeyEvent end) {
	int type = lua_getglobal(L, "move_done");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	new_window(start.subwindow);
	new_point(start.x_root, start.y_root);
	new_point(end.x_root, end.y_root);
	int status = lua_pcall(L, 3, 0, 0);
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
	new_window(start.subwindow);
	new_point(winstart.x, winstart.y);
	new_point(start.x_root, start.y_root);
	new_point(end.x_root, end.y_root);
	int status = lua_pcall(L, 4, 0, 0);
	if (status) {
		printf("Error in lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void luaMoveStart(XKeyEvent start) {
	int type = lua_getglobal(L, "move_start");
	if (!type) {
		lua_pop(L, 1);
		return;
	}
	new_window(start.subwindow);
	new_point(start.x_root, start.y_root);
	int status = lua_pcall(L, 2, 0, 0);
	if (status) {
		printf("Error in lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

static int lf_getAttributes(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "wrong number of arguments");
	int status;
	lua_Integer w = lua_tointegerx(L, 1, &status);
	if (!status)
		return luaL_error(L, "not a number");
	XWindowAttributes attr;
	XGetWindowAttributes(D, w, &attr);
	new_point(attr.x, attr.y);
	new_point(attr.width, attr.height);
	return 2;
}

static int lf_killClient(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "wrong number of arguments");
	int status;
	lua_Integer w = lua_tointegerx(L, 1, &status);
	if (!status) {
		return luaL_error(L, "not a number");
	}
	status = XKillClient(D, (Window)w); //todo: maybe add some kind of hash to window numbers (or make them weird decimals)
	// when they're passed to lua, so there's no chance of accidents from passing random values
	lua_pushboolean(L, status==0);
	return 1;
}

static int point_sub(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "wrong number of arguments");
	int* data = lua_touserdata(L, 1);
	int* data2 = lua_touserdata(L, 2);
	new_point(data[0]-data2[0], data[1]-data2[1]);
	return 1;
}
// todo: we need to check the types here lol
static int point_add(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "wrong number of arguments");
	int* data = lua_touserdata(L, 1);
	int* data2 = lua_touserdata(L, 2);
	new_point(data[0]+data2[0], data[1]+data2[1]);
	return 1;
}

static int point_index(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "wrong number of arguments");
	int* data = lua_touserdata(L, 1);
	const char* field = lua_tostring(L, 2);
	if (!strcmp(field, "x"))
		lua_pushnumber(L, data[0]);
	else if (!strcmp(field, "y"))
		lua_pushnumber(L, data[1]);
	else
		lua_pushnil(L);
	return 1;
}

static int window_index(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "wrong number of arguments");
	Window* data = lua_touserdata(L, 1);
	const char* field = lua_tostring(L, 2);
	XWindowAttributes attr;
	if (!strcmp(field, "pos")) {
		XGetWindowAttributes(D, data[0], &attr);
		new_point(attr.x, attr.y);
	} else if (!strcmp(field, "size")) {
		XGetWindowAttributes(D, data[0], &attr);
		new_point(attr.width, attr.height);
	} else if (!strcmp(field, "close")) {
		lua_pushcfunction(L, window_kill);
	} else if (!strcmp(field, "move")) {
		lua_pushcfunction(L, window_move);
	} else if (!strcmp(field, "focus")) {
		lua_pushcfunction(L, window_focus);
	} else
		lua_pushnil(L);
	return 1;
}

static int window_tostring(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "wrong number of arguments");
	Window* data = lua_touserdata(L, 1);
	lua_pushfstring(L, "Window[%d]", data[0]);
	return 1;
}

static int point_tostring(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "wrong number of arguments");
	int* data = lua_touserdata(L, 1);
	lua_pushfstring(L, "(%d,%d)",data[0],data[1]);
	return 1;
}

static int lf_new_point(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "wrong number of arguments");
	new_point(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 1;
}

void luaInit(Display* d) {
	D = d;
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadfile(L, "script.lua");
	lua_pcall(L, 0, LUA_MULTRET, 0);
 
	luaL_newmetatable(L, "point");
	lua_pushcfunction(L, point_index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, point_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pushcfunction(L, point_sub);
	lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, point_add);
	lua_setfield(L, -2, "__add");

	luaL_newmetatable(L, "window");
	lua_pushcfunction(L, window_index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, window_tostring);
	lua_setfield(L, -2, "__tostring");
	
	lua_register(L, "attr", lf_getAttributes);
	lua_register(L, "P", lf_new_point);
}
