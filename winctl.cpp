#include <cstring>
#include <map>
#include <stdexcept>

#include "winctl.hpp"

#define SPOTIFY_WM_CLASS "spotify"

unsigned char* getXProperty(Display* display, Window win, Atom prop, Atom req_type, unsigned long* length) {
    int form;
    unsigned long remaining;
    Atom type;
    unsigned char* result;

    if (XGetWindowProperty(display, win, prop, 0, 1024, False, req_type, &type, &form, length, &remaining, &result) != Success) {
        throw std::runtime_error("Failed to list the display windows");
        return NULL;
    }
    return result;
}

Atom getXAtom(Display* display, const char* name) {
    static std::map<std::pair<Display*, const char*>, Atom> atoms;
    std::pair<Display*, const char*> key(display, name);
    if (atoms.find(key) != atoms.end()) return atoms[key];
    Atom atom = XInternAtom(display, name, False);
    if (atom == None) {
        throw std::runtime_error("Failed to get an X atom");
    }
    atoms[key] = atom;
    return atom;
}

unsigned long getWindowPid(Display* display, Window win) {
    unsigned char* pid_prop_val;
    unsigned long length;
    unsigned long pid = 0UL;

    Atom pid_prop = getXAtom(display, "_NET_WM_PID");
    pid_prop_val = getXProperty(display, win, pid_prop, XA_CARDINAL, &length);
    if ((length > 0) && pid_prop_val) {
        pid = pid_prop_val[0] + (pid_prop_val[1] << 8) + (pid_prop_val[2] << 16) + (pid_prop_val[3] << 24);
    }

    return pid;
}

bool isSpotifyWindow(Display* display, Window win) {
    unsigned char* winclass;
    unsigned long length;
    bool res = false;

    Atom wm_class_prop = getXAtom(display, "WM_CLASS");
    winclass = getXProperty(display, win, wm_class_prop, XA_STRING, &length);
    if (length > 0) {
        winclass[length - 1] = '\0';
        res = std::strlen((char*)winclass) == std::strlen(SPOTIFY_WM_CLASS) &&
              (std::strncmp((char*)winclass, SPOTIFY_WM_CLASS, strlen(SPOTIFY_WM_CLASS)) == 0);
        XFree(winclass);
    }
    return res;
}

WindowData getSpotifyWindow() {
    Atom win_list_prop;
    Display* display;
    unsigned long length, i;
    Window* win_list;

    WindowData result;

    display = XOpenDisplay(nullptr);

    /* List all the windows the window manager knows abouti. */
    win_list_prop = getXAtom(display, "_NET_CLIENT_LIST");
    win_list = (Window*)getXProperty(display, DefaultRootWindow(display), win_list_prop, XA_WINDOW, &length);
    /* Try to find the one with the WM_CLASS property corresponding
     * to the Spotify client. */
    for (i = 0; i < length; i++) {
        if (isSpotifyWindow(display, win_list[i])) {
            result.wid = win_list[i];
            result.pid = getWindowPid(display, win_list[i]);
            break;
        }
    }
    XFree(win_list);
    return result;
}
