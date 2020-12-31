#include "macros.h"

LUA_CLASSNAME(point);

#define index index_

//temp
static void argCount(int n) {
	if (lua_gettop(L) != n)
		luaL_error(L, "wrong number of arguments");
}

void Lc_point_create(lua_Number x, lua_Number y) {
	Class* data = LUA_NEWUDATA();
	data->x = x;
	data->y = y;
}

LUA_FUNC(sub) {
	LUAOPARGS;
	Lc_point_create(a->x-b->x, a->y-b->y);
	return 1;
}
LUA_FUNC(add) {
	LUAOPARGS;
	Lc_point_create(a->x+b->x, a->y+b->y);
	return 1;
}
LUA_FUNC(mul) {
	LUAOPARGS;
	Lc_point_create(a->x*b->x, a->y*b->y);
	return 1;
}
LUA_FUNC(div) {
	LUAOPARGS;
	Lc_point_create(a->x/b->x, a->y/b->y);
	return 1;
}
LUA_FUNC(eq) {
	LUAOPARGS;
	lua_pushboolean(L, a->x==b->x && a->y==b->y);
	return 1;
}
LUA_FUNC(index) {
	LUASELF;
	LUAPROP(x, lua_pushnumber(L, self->x);)
	LUAPROP(y, lua_pushnumber(L, self->y);)
	LUAENDPROP;
}
LUA_FUNC(construct) {
	argCount(2);
	Lc_point_create(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 1;
}
LUA_FUNC(tostring) {
	argCount(1);
	Lc_point* data = lua_touserdata(L, 1);
	lua_pushfstring(L, "(%f,%f)", data->x, data->y);
	return 1;
}

void Lc_point_init(void) {
	LUACLASS
		LUAOP(index),
		LUAOP(tostring),
		LUAOP(sub),
		LUAOP(add),
		LUAOP(mul),
		LUAOP(div),
		LUAOP(eq),
		LUAENDCLASS;
	lua_register(L, "P", construct);
}
