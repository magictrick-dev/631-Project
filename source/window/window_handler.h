#ifndef J5_WINDOW_HANDLER_H
#define J5_WINDOW_HANDLER_H
#include <core.h>

typedef void* j5win;

bool    window_initialize_startup(void);
bool    window_process_updates(void);

j5win   window_create(const char *window_name, u32 width, u32 height);
void    window_close(j5win *window_handle);

#endif
