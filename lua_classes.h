#include <X11/Xlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

typedef struct {
	lua_Number x;
	lua_Number y;
} Lc_point;
void Lc_point_init(void);
void Lc_point_create(lua_Number x, lua_Number y);

typedef Window Lc_window;
void Lc_window_init(void);
void Lc_window_create(Window w);
