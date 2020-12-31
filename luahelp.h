#include <X11/Xlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

extern Display* D;
extern lua_State* L;

#define LUA_NEWUDATA(class) Lc_##class* data = lua_newuserdata(L, sizeof(Lc_##class)); luaL_setmetatable(L, #class)

#define LUA_GETUDATA(class, var, index) Lc_##class* var = luaL_checkudata(L, index, #class)

#define LUAOPARGS(class) argCount(2); Lc_##class *a = luaL_checkudata(L, 1, #class), *b = luaL_checkudata(L, 2, #class)

#define LUASELF(class) argCount(2); Lc_##class* self = luaL_checkudata(L, 1, #class); const char* field = lua_tostring(L, 2);
#define LUAPROP(name, action) if (!strcmp(field, #name)) { action } else
#define LUAMETHOD(class, name) if (!strcmp(field, #name)) { lua_pushcfunction(L, class##_##name); } else
#define LUAENDPROP { luaL_error(L, "unknown property"); } return 1;

#define LUACLASS(class) luaL_newmetatable(L, #class); luaL_setfuncs(L, (luaL_Reg[]){
#define LUAOP(class, name) {"__" #name, class##_##name }
#define LUAENDCLASS {NULL, NULL}}, 0); lua_pop(L, 1)

typedef struct {
	lua_Number x;
	lua_Number y;
} Lc_point;
void Lc_point_init(void);
void Lc_point_create(lua_Number x, lua_Number y);

typedef Window Lc_window;
void Lc_window_init(void);
void Lc_window_create(Window w);
