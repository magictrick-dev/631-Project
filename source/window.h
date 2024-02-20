#ifndef J5_WINDOW_H
#define J5_WINDOW_H
#include <windows.h>

struct window_state
{
    int     width;
    int     height;
    HWND    handle;
    HDC     device;
};

window_state * get_window_state(void);

#endif
