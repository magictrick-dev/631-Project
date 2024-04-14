#ifndef J5_RENDERER_H
#define J5_RENDERER_H
#include <vector>
#include <core.h>
#include <renderer/device.h>

#define ATTR_CONSTANT   4
#define ATTR_R          5
#define ATTR_G          6
#define ATTR_B          7
#define ATTR_NX         8
#define ATTR_NY         9
#define ATTR_NZ         10
#define ATTR_S          11
#define ATTR_T          12
#define ATTR_WORLD_X    13
#define ATTR_WORLD_Y    14
#define ATTR_WORLD_Z    15

#define ATTR_SIZE       16

struct attr_point
{
    union
    {

        f32 coord[16];

        struct
        {
            v4      position;
            f32     constant;
            v3      color;
            v3      normals;
            f32     s;
            f32     t;
            v3      world;
        };

    };
};

inline attr_point
interpolate_attributed_point(attr_point a, attr_point b, f32 t)
{
    
    attr_point result = {};

    for (size_t i = 0; i < ATTR_SIZE; ++i)
        result.coord[i] = a.coord[i] + (t * (b.coord[i] - a.coord[i]));

    return result;

}

// --- Renderer Functions ------------------------------------------------------
//
// Color values are normalized 0.0-1.0f.
// 



void    set_point(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
void    set_fill(renderable_device *device, v3 color);
void    set_line(renderable_device *device, i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color);
void    set_circle(renderable_device *device, i32 x, i32 y, i32 z, i32 r, v3 color);
void    set_flood(renderable_device *device, i32 x, i32 y, i32 z, v3 color);
f32*    get_depthbuffer(u32 width, u32 height);
void    create_depthbuffer(u32 width, u32 height);
bool    set_depthbuffer(u32 x, u32 y, f32 value);
void    clear_depthbuffer();
bool    line_clip(v4 *a, v4 *b);
void    set_line_dda(renderable_device *device, v4 a, v4 b, v3 color);

i32     poly_clip(std::vector<attr_point>& vertex_list, std::vector<attr_point>& clipped_list);
void    scan_convert(std::vector<attr_point>& clip_list);

#endif
