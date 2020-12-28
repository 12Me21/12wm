#include <X11/Xlib.h>

#define loop for (;;)
#define var __auto_type

int main(void) {
	var dpy = XOpenDisplay(0);
	if (!dpy) return 1;
	var root = DefaultRootWindow(dpy);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Super_L")), AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
	XKeyEvent start;
	start.subwindow = None;
	Bool resize = False;
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
