
#include <stdio.h>

#include "globals.h"

struct pinta_x xinfo;

void
pinta_x_destroy()
{
    XDestroyWindow(xinfo.xdisplay, xinfo.win);
    XCloseDisplay(xinfo.xdisplay);
}

int
pinta_x_init(int w, int h)
{
    Window root;
    XSetWindowAttributes swa;

    // Open standard display (primary screen)
    xinfo.xdisplay = XOpenDisplay(NULL);
    if (xinfo.xdisplay == NULL) {
        sprintf(errstr, "Error opening X display");
        return -1;
    }

    // Get the root window (usually the whole screen)
    root = DefaultRootWindow(xinfo.xdisplay);

    // list all events this window accepts
    swa.event_mask = ExposureMask
        | KeyPressMask
        | ButtonPressMask;

    // Xlib's window creation
    xinfo.win = XCreateWindow(xinfo.xdisplay, root, 0, 0, w, h, 0,
        CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

    // Make window visible
    XMapWindow(xinfo.xdisplay, xinfo.win);

    return 0;
}

