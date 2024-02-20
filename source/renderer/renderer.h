#ifndef J5_RENDERER_H
#define J5_RENDERER_H
#include <core.h>

// --- Renderer Functions ------------------------------------------------------
//
// Color values are normalized 0.0-1.0f.
//
// NOTE(Chris): The set_fill() function draws over the contents of the entire
//              window using the provided color using StretchDIBits() and doesn't
//              need bounds checking. It is faster than manually blitting over
//              the whole window.
// 
// NOTE(Chris): The set_pixel() function doesn't perform bounds checking. It is
//              the responsibility of the caller to ensure that the location is
//              within bounds of the device it is drawing to.
//

void set_pixel(i32 x, i32 y, f32 r, f32 g, f32 b);
void set_fill(f32 r, f32 g, f32 b);
void set_line(i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, f32 r, f32 g, f32 b);

#endif
