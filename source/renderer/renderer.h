#ifndef J5_RENDERER_H
#define J5_RENDERER_H
#include <core.h>

// --- Renderer Functions ------------------------------------------------------
//
// Color values are normalized 0.0-1.0f.
// 

void set_pixel(i32 x, i32 y, v3   color);
void get_pixel(i32 x, i32 y, u32 *color);

void set_fill(v3 color);
void set_line(i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color);
void set_circle(i32 x, i32 y, i32 z, i32 r, v3 color);
void set_flood(i32 x, i32 y, i32 z, v3 color);

#endif
