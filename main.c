#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>

#include "lua.h"

#define loop for (;;)
#define var __auto_type
#define when(x) break; case (x)

static Display* D;

static void focusWindow(Window w) {
	XSetInputFocus(D, w, RevertToPointerRoot, CurrentTime);
	XRaiseWindow(D, w);
}

static Window getFocused() {
	Window w;
	int x;
	XGetInputFocus(D, &w, &x);
	return w;
}

/*static void focusNextWindow(Display* d) {
	Window *children, focused;
	unsigned int i, nchildren;
	if (!(XQueryTree(d, DefaultRootWindow(d), NULL, NULL, &children, &nchildren))) return;
	XGetInputFocus(d, &focused, NULL);
	for (i=0; i<nchildren; i++)
		if (children[i] == focused)
			break;
	i = (i+1) % nchildren;
	focusWindow(d, children[i]);
	XFree(children);
}*/

int main(void) {
	D = XOpenDisplay(NULL);
	if (!D) return 1;
	luaInit(D);
	var root = DefaultRootWindow(D);
	KeyCode win = XKeysymToKeycode(D, XK_Super_L);
	// disable "focus follows mouse"
	XSetInputFocus(D, root, RevertToNone, CurrentTime);
	// listen for windows key keypresses. this also records any key which is pressed while that key is held.
	XGrabKey(D, win, AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
	// listen for leftclicks
	XGrabButton(D, Button1, AnyModifier, root, False, ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);
	
	XKeyEvent start = {
		.subwindow = None
	};
	Bool resize = False;
	XWindowAttributes attr;
	XEvent ev;
	loop {
		XAllowEvents(D, ReplayPointer, CurrentTime);
		XNextEvent(D, &ev);
		switch (ev.type) {
		when(ButtonPress):
			luaClick(ev.xbutton);
		when(KeyPress):
			if (ev.xkey.keycode == win) {
				start = ev.xkey;
				luaMoveStart(start);
				if (start.subwindow) {
					XGetWindowAttributes(D, start.subwindow, &attr);
					XGrabPointer(D, root, True, PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
				}
			} else {
				luaKeyPress(ev.xkey);
			}
		when(KeyRelease):
			if (ev.xkey.keycode == win) {
				luaMoveDone(start, ev.xkey, attr);
				XUngrabPointer(D, CurrentTime);
			} else {
				//luaKeyPress(ev.xkey);
			}
		when(MotionNotify):
			luaMoveDuring(start, ev.xkey, attr);
		}
	}
}
