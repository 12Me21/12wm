#include <X11/Xlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

static lua_State* L;

void luaProcessEvent(XEvent event) {
	
}

void luaInit(void) {
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadfile(L, "script.lua");
	lua_pcall(L, 0, LUA_MULTRET, 0);
}
