#include <X11/Xlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#include "classes.h" //not great to include this since it increases recompilations

int strcmp(const char *s1, const char *s2);

extern Display* D;
extern lua_State* L;

#define LUA_CLASSNAME(name) typedef Lc_##name Class; static const char* class_name = #name

#define LUA_NEWUDATA() lua_newuserdata(L, sizeof(Class)); luaL_setmetatable(L, class_name)

#define LUA_GETUDATA(index) luaL_checkudata(L, index, class_name)

#define LUAOPARGS argCount(2); Class *a = luaL_checkudata(L, 1, class_name), *b = luaL_checkudata(L, 2, class_name)

#define LUASELF argCount(2); Class* self = luaL_checkudata(L, 1, class_name); const char* field = lua_tostring(L, 2);
#define LUAPROP(name, action) if (!strcmp(field, #name)) { action } else
#define LUAMETHOD(name) if (!strcmp(field, #name)) { lua_pushcfunction(L, m##_##name); } else
#define LUAENDPROP { luaL_error(L, "unknown property"); } return 1;

#define LUA_FUNC(name) static int name(lua_State* L)

#define LUACLASS luaL_newmetatable(L, class_name); luaL_setfuncs(L, (luaL_Reg[]){
#define LUAOP(name) {"__" #name, name }
#define LUAENDCLASS {NULL, NULL}}, 0); lua_pop(L, 1)
