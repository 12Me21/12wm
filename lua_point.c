#include <string.h>
#include "luahelp.h"

//typedef Lc_point class_t;

//temp
static void argCount(int n) {
	if (lua_gettop(L) != n)
		luaL_error(L, "wrong number of arguments");
}

void Lc_point_create(lua_Number x, lua_Number y) {
	LUA_NEWUDATA(point);
	data->x = x;
	data->y = y;
}

static int point_sub(lua_State* L) {
	LUAOPARGS(point);
	Lc_point_create(a->x-b->x, a->y-b->y);
	return 1;
}
static int point_add(lua_State* L) {
	LUAOPARGS(point);
	Lc_point_create(a->x+b->x, a->y+b->y);
	return 1;
}
static int point_mul(lua_State* L) {
	LUAOPARGS(point);
	Lc_point_create(a->x*b->x, a->y*b->y);
	return 1;
}
static int point_div(lua_State* L) {
	LUAOPARGS(point);
	Lc_point_create(a->x/b->x, a->y/b->y);
	return 1;
}
static int point_eq(lua_State* L) {
	LUAOPARGS(point);
	lua_pushboolean(L, a->x==b->x && a->y==b->y);
	return 1;
}

static int point_index(lua_State* L) {
	LUASELF(point);
	LUAPROP(x, lua_pushnumber(L, self->x);)
	LUAPROP(y, lua_pushnumber(L, self->y);)
	LUAENDPROP;
}

static int point_construct(lua_State* L) {
	argCount(2);
	Lc_point_create(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 1;
}

static int point_tostring(lua_State* L) {
	argCount(1);
	Lc_point* data = lua_touserdata(L, 1);
	lua_pushfstring(L, "(%f,%f)", data->x, data->y);
	return 1;
}

void Lc_point_init(void) {
	LUACLASS(point)
		LUAOP(point, index),
		LUAOP(point, tostring),
		LUAOP(point, sub),
		LUAOP(point, add),
		LUAOP(point, mul),
		LUAOP(point, div),
		LUAOP(point, eq),
		LUAENDCLASS;
	lua_register(L, "P", point_construct);
}
