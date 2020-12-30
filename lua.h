#include <X11/Xlib.h>

void luaKeyPress(XKeyEvent e);
void luaMoveDone(XKeyEvent e, XKeyEvent f, XWindowAttributes w);
void luaMoveDuring(XKeyEvent e, XKeyEvent f, XWindowAttributes w);
void luaMoveStart(XKeyEvent start);
void luaClick(XButtonEvent start);

void luaInit(Display* d);
