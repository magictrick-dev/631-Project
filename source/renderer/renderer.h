#ifndef J5_RENDERER_H
#define J5_RENDERER_H
#include <core.h>
#include <renderer/device.h>

// --- Renderer Functions ------------------------------------------------------
//
// Color values are normalized 0.0-1.0f.
// 

//void    set_pixel(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
//u32     get_pixel(renderable_device *device, i32 x, i32 y, i32 z);

void set_point(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
void set_fill(renderable_device *device, v3 color);
void set_line(renderable_device *device, i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color);
void set_circle(renderable_device *device, i32 x, i32 y, i32 z, i32 r, v3 color);
void set_flood(renderable_device *device, i32 x, i32 y, i32 z, v3 color);

#endif
