#include "luahelp.h"

LUA_CLASSNAME(window);

static void argCount(int n) {
	if (lua_gettop(L) != n)
		luaL_error(L, "wrong number of arguments");
}

void Lc_window_create(Window w) {
	if (!w) {
		lua_pushnil(L);
		return;
	}
	Class* data = LUA_NEWUDATA();
	*data = w;
}

LUA_FUNC(m_close) {
	argCount(1);
	Class* data = LUA_GETUDATA(1);
	int status = XKillClient(D, *data);
	lua_pushboolean(L, status==0);
	return 1;
}

LUA_FUNC(m_focus) {
	argCount(1);
	Class* data = LUA_GETUDATA(1);
	int status = XSetInputFocus(D, *data, RevertToPointerRoot, CurrentTime);
	XRaiseWindow(D, *data);
	lua_pushboolean(L, status==0);
	return 1;
}

LUA_FUNC(m_move) {
	int n = lua_gettop(L);
	if (n != 2 && n!=3)
		return luaL_error(L, "wrong number of arguments");
	Class* w = LUA_GETUDATA(1);
	Lc_point* p = luaL_checkudata(L, 2, "point");
	int status;
	if (n==3) {
		Lc_point* r = luaL_checkudata(L, 3, "point");
		status = XMoveResizeWindow(D, w[0], p->x, p->y, r->x, r->y);
	} else {
		status = XMoveWindow(D, w[0], p->x, p->y);
	}
	lua_pushboolean(L, status==0);
	return 1;
}

LUA_FUNC(m_resize) {
	argCount(2);
	Class* w = LUA_GETUDATA(1);
	Lc_point* p = luaL_checkudata(L, 2, "point");
	int status = XResizeWindow(D, *w, p->x, p->y);
	lua_pushboolean(L, status==0);
	return 1;
}

LUA_FUNC(index) {
	LUASELF;
	XWindowAttributes attr;
	LUAPROP(pos,
		XGetWindowAttributes(D, *self, &attr);
		Lc_point_create(attr.x, attr.y);
	)
	LUAPROP(size,
		XGetWindowAttributes(D, *self, &attr);
		Lc_point_create(attr.width, attr.height);
	)
	LUAMETHOD(close)
	LUAMETHOD(move)
	LUAMETHOD(focus)
	LUAMETHOD(resize)
	LUAENDPROP;
}

LUA_FUNC(tostring) {
	argCount(1);
	Class* data = LUA_GETUDATA(1);
	lua_pushfstring(L, "Window[%d]", data[0]);
	return 1;
}

void Lc_window_init(void) {
	LUACLASS
		LUAOP(index),
		LUAOP(tostring),
	LUAENDCLASS;
}
