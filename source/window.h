// --- J5 Window API -----------------------------------------------------------
// 
// The window API is designed to simple to use on the front end, hiding most of
// gritty stuff that happens behind the scenes. The given API allows for multiple
// windows, HOWEVER, there is no mechanism that directly differentiates which window
// receives which message, so there's no point to have multiple windows.
//

#ifndef J5_WINDOW_H
#define J5_WINDOW_H
#include <core.h>

typedef void* j5_window;

j5_window   window_create(const char *window_title, i32 width, i32 height);
bool        window_update(j5_window window);

void        window_close(j5_window window);

i32         window_width(j5_window window);
i32         window_height(j5_window window);

#endif
