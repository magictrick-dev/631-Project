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

// --- Light Modeling ----------------------------------------------------------

struct farlight
{
    v3 I;
    v3 C;
    f32 intensity;
};

struct ambientlight
{
    v3 C;
    f32 intensity;
};

struct pointlight
{
    v3 P;
    v3 C;
    f32 intensity;
};

struct light_model;
typedef void (*surface_shader_fptr)(v3& color, light_model& model);
void matte_shader(v3& color, light_model& model);
void metal_shader(v3& color, light_model& model);
void plastic_shader(v3& color, light_model& model);

struct light_model
{
    f32 diffuse_coefficient     = 1.0f;
    f32 ambient_coefficient     = 0.0f;
    f32 specular_coefficient    = 0.0f;
    v3  surface_color           = { 1.0f, 1.0f, 1.0f };
    v3  specular_color          = { 1.0f, 1.0f, 1.0f };
    f32 specular_exponent       = 10.0f;

    bool vertex_color_flag          = false;
    bool vertex_normal_flag         = false;
    bool vertex_texture_flag        = false;
    bool vertex_interpolate_flag    = true;

    v4 camera_eye = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Only one ambient light, many far and point lights.
    ambientlight ambient            = { 1.0f, 1.0f, 1.0f, 1.0f };
    std::vector<farlight>   farlights;
    std::vector<pointlight> pointlights;

    m4 light_transform = m4::create_identity();
    std::vector<m4> transform_stack;

    v4  view_vector;
    v4  poly_normal;
    attr_point surface_point_values;

    surface_shader_fptr shader_function = matte_shader;

};

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
void    create_edgetable(u32 height);
void    scan_convert(renderable_device* device, std::vector<attr_point>& clip_list,
        v3 color, light_model& lighting);

#endif
