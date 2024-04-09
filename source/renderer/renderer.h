#ifndef J5_RENDERER_H
#define J5_RENDERER_H
#include <core.h>
#include <renderer/device.h>

struct dda_vertex
{
    v4 position;
};

// --- Renderer Functions ------------------------------------------------------
//
// Color values are normalized 0.0-1.0f.
// 

//void    set_pixel(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
//u32     get_pixel(renderable_device *device, i32 x, i32 y, i32 z);

void    set_point(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
void    set_fill(renderable_device *device, v3 color);
void    set_line(renderable_device *device, i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color);
void    set_circle(renderable_device *device, i32 x, i32 y, i32 z, i32 r, v3 color);
void    set_flood(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
f32*    get_depthbuffer(u32 width, u32 height);
void    create_depthbuffer(u32 width, u32 height);
void    clear_depthbuffer();
bool    line_clip(v4 *a, v4 *b);
void    set_line_dda(renderable_device *device, dda_vertex v1, dda_vertex v2, v3 color, m4 otw, m4 wtc, m4 ctc, m4 ctd);

#endif
