#ifndef WINCTL_HPP
#define WINCTL_HPP

#include <X11/Xatom.h>
#include <X11/Xlib.h>

// Window data
struct WindowData {
    WindowData(): wid(0), pid(0) {}
    Window wid;
    unsigned long pid;
    operator bool() const { return wid != 0; }
};

// Helper function to retrieve a X11 window property: display is the display
// of the window win, prop is the requested property of type req_type. The
// result should be cast to the desired type and its length is stored at the
// lenght pointer.
unsigned char* getXProperty(Display* display, Window win, Atom prop, Atom req_type, unsigned long* length);

// Get xlib atom, caching the result.
Atom getXAtom(Display* display, const char* name);

// For the given X11 window on given display, get its PID
unsigned long getWindowPid(Display* display, Window win);

// For the given X11 window on given display, find its WM_CLASS property and
// return true if it corresponds to the Spotify client application.
bool isSpotifyWindow(Display* display, Window win);

// Use X11 to find the Spotify client window and get a GDK window for it.
// Returns the GdkWindow if found, NULL otherwise.
WindowData getSpotifyWindow();

#endif
