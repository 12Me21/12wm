#define _GNU_SOURCE 1
#include <X11/Xlib.h>
#include <lua5.3/lua.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define loop for (;;)
#define var __auto_type

/*static void getQuadrant(XWindowAttributes attr, XEvent evt) {
	int x = evt.x - attr.x;
	int y = evt.y - attr.y;
	if (x < attr.width/3) x = -1;
	else if (x > attr.width*2/3) x = 1;
	else x = 0;
	if (y < attr.height/3) y = -1;
	else if (y > attr.width*2/3) y = 1;
	else y = 0;
	
	}*/

static KeyCode key (Display* d, char * name) {
	return XKeysymToKeycode(d, XStringToKeysym(name));
}

static void focusWindow(Display* d, Window w) {
	XSetInputFocus(d, w, RevertToPointerRoot, CurrentTime);
	XRaiseWindow(d, w);
}

static Window getFocused(Display* d) {
	Window w;
	int x;
	XGetInputFocus(d, &w, &x);
	return w;
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
	focusWindow(d, children[i]);
	XFree(children);
}

handleKey() {
	
}

void handler(int signum) {
	puts("SIGMA");
}

int main(void) {
	int z;
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	z|=sigaction(SIGIO, &action, NULL);
	
	int ifd = inotify_init();
	z|=inotify_add_watch(ifd, "script.lua", IN_CLOSE_WRITE);
	
	z|=fcntl(ifd, F_SETFL, O_ASYNC);
	z|=fcntl(ifd, F_SETOWN, getpid());
	z|=fcntl(ifd, F_SETSIG, 0);
	printf("%d\n",z);

	var dpy = XOpenDisplay(0);
	if (!dpy) return 1;
	var root = DefaultRootWindow(dpy);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Super_L")), AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Tab")), Mod3Mask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("X")), Mod3Mask, root, True, GrabModeAsync, GrabModeAsync);
	XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
	XSetInputFocus(dpy, root, RevertToNone, CurrentTime);
	XGrabButton(dpy, Button1, AnyModifier, root, False, ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);
	//XSelectInput(dpy, root, ButtonPressMask);
	
	XKeyEvent start;
	start.subwindow = None;
	Bool resize = False;
	int qx, qy;
	loop {
		XWindowAttributes attr;
		XEvent ev;
		XAllowEvents(dpy, ReplayPointer, CurrentTime);
		//todo: peekevent + nextevent if needed
		if (XCheckTypedEvent(dpy, ButtonPress, &ev)) {
			if (getFocused(dpy) != ev.xbutton.subwindow)
				focusWindow(dpy, ev.xbutton.subwindow);
		} else if (XCheckTypedEvent(dpy, KeyPress, &ev)) {
			start = ev.xkey;
			puts("key");
			if (ev.xkey.keycode == key(dpy, "Super_L")) {
				if (start.subwindow) {
					focusWindow(dpy, start.subwindow);
					XGetWindowAttributes(dpy, start.subwindow, &attr);
					resize = !(ev.xbutton.x_root - attr.x < attr.width - 50 && ev.xbutton.y_root - attr.y < attr.height - 50);
					XGrabPointer(dpy, root, True, PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
				}
			} else if (ev.xkey.keycode == key(dpy, "Q")) {
				if (start.subwindow)
					XKillClient(dpy, start.subwindow);
			} else if (ev.xkey.keycode == key(dpy, "Z")) {
				//focusNextWindow(dpy);
			}
		} else if (XCheckTypedEvent(dpy, KeyRelease, &ev)) {
			if (start.subwindow) {
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
			}
			XUngrabPointer(dpy, CurrentTime);
		} else if (XCheckTypedEvent(dpy, MotionNotify, &ev)) {
			if (start.subwindow) {
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
		}
	}
}
