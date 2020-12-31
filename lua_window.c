#include "luahelp.h"

static void argCount(int n) {
	if (lua_gettop(L) != n)
		luaL_error(L, "wrong number of arguments");
}

void Lc_window_create(Window w) {
	if (!w) {
		lua_pushnil(L);
		return;
	}
	LUA_NEWUDATA(window);
	*data = w;
}

static int window_close(lua_State* L) {
	argCount(1);
	Window* data = luaL_checkudata(L, 1, "window");
	int status = XKillClient(D, data[0]);
	lua_pushboolean(L, status==0);
	return 1;
}

static int window_focus(lua_State* L) {
	argCount(1);
	Window* data = luaL_checkudata(L, 1, "window");
	int status = XSetInputFocus(D, data[0], RevertToPointerRoot, CurrentTime);
	XRaiseWindow(D, data[0]);
	lua_pushboolean(L, status==0);
	return 1;
}

static int window_move(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 2 && n!=3)
		return luaL_error(L, "wrong number of arguments");
	Window* w = lua_touserdata(L, 1);
	Lc_point* p = lua_touserdata(L, 2);
	int status;
	if (n==3) {
		Lc_point* r = lua_touserdata(L, 3);
		status = XMoveResizeWindow(D, w[0], p->x, p->y, r->x, r->y);
	} else {
		status = XMoveWindow(D, w[0], p->x, p->y);
	}
	lua_pushboolean(L, status==0);
	return 1;
}

static int window_resize(lua_State* L) {
	argCount(2);
	Window* w = lua_touserdata(L, 1);
	Lc_point* p = lua_touserdata(L, 2);
	int status = XResizeWindow(D, w[0], p->x, p->y);
	lua_pushboolean(L, status==0);
	return 1;
}

static int window_index(lua_State* L) {
	LUASELF(window);
	XWindowAttributes attr;
	LUAPROP(pos,
		XGetWindowAttributes(D, *self, &attr);
		Lc_point_create(attr.x, attr.y);
	)
	LUAPROP(size,
		XGetWindowAttributes(D, *self, &attr);
		Lc_point_create(attr.width, attr.height);
	)
	LUAMETHOD(window, close)
	LUAMETHOD(window, move)
	LUAMETHOD(window, focus)
	LUAMETHOD(window, resize)
	LUAENDPROP;
}

static int window_tostring(lua_State* L) {
	argCount(1);
	LUA_GETUDATA(window, data, 1);
	lua_pushfstring(L, "Window[%d]", data[0]);
	return 1;
}


void Lc_window_init(void) {
	LUACLASS(window)
		LUAOP(window, index),
		LUAOP(window, tostring),
		LUAENDCLASS;
}
