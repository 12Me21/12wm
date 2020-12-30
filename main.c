#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>

#include "lua.h"

#define loop for (;;)
#define var __auto_type
#define when(x) break; case (x)

Display* D;

static KeyCode key (char * name) {
	return XKeysymToKeycode(D, XStringToKeysym(name));
}

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
	luaInit();
	
	D = XOpenDisplay(NULL);
	if (!D) return 1;
	var root = DefaultRootWindow(D);
	// disable "focus follows mouse"
	XSetInputFocus(D, root, RevertToNone, CurrentTime);
	// listen for windows key keypresses. this also records any key which is pressed while that key is held.
	XGrabKey(D, XKeysymToKeycode(D, XK_Super_L), AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
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
		luaProcessEvent(D, ev);
		switch (ev.type) {
		when(ButtonPress):
			if (getFocused() != ev.xbutton.subwindow)
				focusWindow(ev.xbutton.subwindow);
		when(KeyPress):
			start = ev.xkey;
			if (ev.xkey.keycode == key("Super_L")) {
				if (start.subwindow) {
					focusWindow(start.subwindow);
					XGetWindowAttributes(D, start.subwindow, &attr);
					resize = !(ev.xbutton.x_root - attr.x < attr.width - 50 && ev.xbutton.y_root - attr.y < attr.height - 50);
					XGrabPointer(D, root, True, PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
				}
			} else if (ev.xkey.keycode == key("Z")) {
				//focusNextWindow(D);
			}
		when(KeyRelease):
			if (start.subwindow) {
				if (resize) {
					int xdiff = ev.xbutton.x_root - start.x_root;
					int ydiff = ev.xbutton.y_root - start.y_root;
					XMoveResizeWindow(
						D, start.subwindow,
						attr.x, attr.y,
						attr.width + xdiff, attr.height + ydiff);
				}
				start.subwindow = None;
			}
			XUngrabPointer(D, CurrentTime);
		when(MotionNotify):
			if (start.subwindow) {
				int xdiff = ev.xbutton.x_root - start.x_root;
				int ydiff = ev.xbutton.y_root - start.y_root;
				if (!resize) {
					XMoveResizeWindow(
						D, start.subwindow,
						attr.x + xdiff, attr.y + ydiff,
						attr.width, attr.height);
				}
			}
		}
	}
}
