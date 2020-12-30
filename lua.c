#include <X11/Xlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

extern Display* D;

static lua_State* L;

void luaProcessEvent(Display* D, XEvent event) {
	if (event.type == KeyPress) {
		char* name = XKeysymToString(XKeycodeToKeysym(D, event.xkey.keycode, 0));
		lua_pushvalue(L, -1);
		lua_pushstring(L, name);
		lua_pushinteger(L, event.xkey.subwindow);
		int status = lua_pcall(L, 2, 0, 0);
	}
}

static int lf_killClient(lua_State* L) {
	int n = lua_gettop(L);
	if (!n) {
		return luaL_error(L, "wrong number of arguments");
	}
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

void luaInit(void) {
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadfile(L, "script.lua");
	lua_pcall(L, 0, LUA_MULTRET, 0);
	lua_register(L, "kill", lf_killClient);
}
