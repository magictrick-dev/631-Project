#ifndef J5_RENDERER_H
#define J5_RENDERER_H
#include <core.h>

// --- Render Device -----------------------------------------------------------
//
// The render device allows output to different media (display or images).
//

typedef void    (*device_set_pixel_fptr)(i32 x, i32 y, v3 color);

struct device
{
    i32 width;
    i32 height;

    device_set_pixel_fptr set_pixel;
};

class device
{
    public:
        virtual inline void set_pixel(v2 where, v4 color) = NULL;
        virtual inline void clear(v4 color) = NULL;
        virtual inl.void init(void) = NULL;
        
};

class window : public device
{
    public:
        window(i32 width, i32 height);

        virtual void set_pixel(v2 where, v4 color);
};

// --- Renderer Functions ------------------------------------------------------
//
// Color values are normalized 0.0-1.0f.
// 

void set_pixel(i32 x, i32 y, v3 color);
void set_fill(v3 color);
void set_line(i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color);

#endif
