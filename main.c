#include <X11/Xlib.h>

#define loop for (;;)
#define var __auto_type

static void getQuadrant(XWindowAttributes attr, XEvent evt) {
	int x = evt.x - attr.x;
	int y = evt.y - attr.y;
	if (x < attr.width/3) x = -1;
	else if (x > attr.width*2/3) x = 1;
	else x = 0;
	if (y < attr.height/3) y = -1;
	else if (y > attr.width*2/3) y = 1;
	else y = 0;
	
}

static void focusNextWindow(Display* d) {
	Window *children, focused;
	unsigned int i, nchildren;
	if (!(XQueryTree(d, DefaultRootWindow(d), NULL, NULL, &children, &nchildren))) return;
	XGetInputFocus(d, &focused, NULL);
	for (i=0; i<nchildren; i++)
		if (children[i] == focused)
			break;
	i = (i+1) % nchildren;
	XSetInputFocus(d, children[i], RevertToParent, CurrentTime);
	XFree(children);
}

int main(void) {
	var dpy = XOpenDisplay(0);
	if (!dpy) return 1;
	var root = DefaultRootWindow(dpy);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Super_L")), AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Tab")), Mod3Mask, root, True, GrabModeAsync, GrabModeAsync);
	XKeyEvent start;
	start.subwindow = None;
	Bool resize = False;
	int qx, qy;
	loop {
		XWindowAttributes attr;
		XEvent ev;
		XNextEvent(dpy, &ev);
		if (ev.type==KeyPress && ev.xkey.subwindow) {
			start = ev.xkey;
			XRaiseWindow(dpy, start.subwindow);
			XGetWindowAttributes(dpy, start.subwindow, &attr);
			resize = !(ev.xbutton.x_root - attr.x < attr.width - 50 && ev.xbutton.y_root - attr.y < attr.height - 50);
			XGrabPointer(dpy, root, True, PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
		} else if (ev.type==KeyRelease) {
			if (resize) {
				int xdiff = ev.xbutton.x_root - start.x_root;
				int ydiff = ev.xbutton.y_root - start.y_root;
				XMoveResizeWindow(
					dpy, start.subwindow,
					attr.x,
					attr.y,
					attr.width + xdiff,
					attr.height + ydiff);
			}
			start.subwindow = None;

			XUngrabPointer(dpy, CurrentTime);
		} else if (ev.type == MotionNotify && start.subwindow) {
			int xdiff = ev.xbutton.x_root - start.x_root;
			int ydiff = ev.xbutton.y_root - start.y_root;
			if (!resize) {
				XMoveResizeWindow(
					dpy, start.subwindow,
					attr.x + xdiff,
					attr.y + ydiff,
					attr.width,
					attr.height);
			}
		}
		// idea: resize happens on keyup
		// if another key is held during the keyup,
		// it does resize (and reverts position)
		// use position as resize preview!!
	}
}
