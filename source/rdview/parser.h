#ifndef J5_RDVIEW_PARSER_H
#define J5_RDVIEW_PARSER_H
#include <core.h>

#include <iostream>
#include <vector>
#include <string>

#define RDVIEW_DEVICE_WINDOW        0
#define RDVIEW_DEVICE_PNM           1
#define RDVIEW_DEVICE_BMP           2

#define RDVIEW_DRAW_TYPE_OUTPUT     0
#define RDVIEW_DRAW_TYPE_SINGLE     0
#define RDVIEW_DRAW_TYPE_DOUBLE     1
#define RDVIEW_DRAW_TYPE_STEP       2

#define RDVIEW_OPTYPE_UNDEFINED     0
#define RDVIEW_OPTYPE_DISPLAY       1
#define RDVIEW_OPTYPE_FORMAT        2
#define RDVIEW_OPTYPE_BACKGROUND    3
#define RDVIEW_OPTYPE_COLOR         4
#define RDVIEW_OPTYPE_POINT         5
#define RDVIEW_OPTYPE_LINE          6
#define RDVIEW_OPTYPE_CIRCLE        7
#define RDVIEW_OPTYPE_FLOOD         8
#define RDVIEW_OPTYPE_WORLDBEGIN    9
#define RDVIEW_OPTYPE_WORLDEND      10
#define RDVIEW_OPTYPE_CAMERAEYE     11
#define RDVIEW_OPTYPE_CAMERAAT      12
#define RDVIEW_OPTYPE_CAMERAUP      13
#define RDVIEW_OPTYPE_TRANSLATION   14
#define RDVIEW_OPTYPE_SCALE         15
#define RDVIEW_OPTYPE_ROTATION      16
#define RDVIEW_OPTYPE_CUBE          17
#define RDVIEW_OPTYPE_CAMERAFOV     18
#define RDVIEW_OPTYPE_XFORMPUSH     19
#define RDVIEW_OPTYPE_XFORMPOP      20
#define RDVIEW_OPTYPE_SPHERE        21
#define RDVIEW_OPTYPE_OBJECTBEGIN   22
#define RDVIEW_OPTYPE_OBJECTEND     23
#define RDVIEW_OPTYPE_OBJECTINST    24
#define RDVIEW_OPTYPE_POLYSET       25
#define RDVIEW_OPTYPE_CONE          26
#define RDVIEW_OPTYPE_CYLINDER      27
#define RDVIEW_OPTYPE_FRAMEBEGIN    28
#define RDVIEW_OPTYPE_FRAMEEND      29
#define RDVIEW_OPTYPE_POINTSET      30
#define RDVIEW_OPTYPE_CLIPPING      31

#include <rdview/operations.h>
#include <rdview/statement.h>
#include <renderer/device.h>
#include <renderer/renderer.h>

void        rdview_parser_strip_whitespace(std::string &line);
void        rdview_parser_rejoin_strings(std::vector<std::string>& lk_line);
std::string rdview_parser_keyword_dequote(std::string keyword);

// --- RDView Parser & Configuration -------------------------------------------

typedef void (*surface_shader_fptr)(v3& color);

inline void
matte_shader(v3& color)
{

}

inline void
metal_shader(v3& color)
{

}

inline void
plastic_shader(v3& color)
{
    
}

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

    v4  view_vector;
    v4  poly_normal;
    attr_point surface_point_values;

    surface_shader_fptr shader_function = matte_shader;

};

class rdview
{
    public:
        static char *   get_source(const char *file_path);
        static void     free_source(char *source_buffer);

    public:
        rdview(const char *rdview_source_code);
        bool init();
        bool begin();
        void render();

        rdoperation* create_operation(rdstatement* stm);

    public:
        void rd_point_pipeline(v3 point, bool move);
        void rd_line_pipeline(v3 point, bool move);
        void rd_poly_pipeline(attr_point p, bool end_flag);

        v4  rd_line_state;
        v4  rd_point_state;

    public:
        i32 width        = 640;
        i32 height       = 480;
        v3  draw_color   = { 1.0f, 1.0f, 1.0f };
        v3  canvas_color = { 0.0f, 0.0f, 0.0f };
    
        v3  camera_eye  = { 0, 0, 0 };
        v3  camera_at   = { 0, 0, -1 };
        v3  camera_up   = { 0, 1, 0 };

        light_model lighting;

        f32 fov = 90.0f;
        f32 nearp = 1.0f;
        f32 farp = 100000000.0f;

        m4 world_to_camera;
        m4 camera_to_clip;
        m4 clip_to_device;

        size_t parse_index = 0;

        m4 current_transform;
        std::vector<m4> transform_stack;
        std::vector<m4> light_transform_stack;

        std::string title;
        u32         device;
        u32         mode;

        renderable_device *active_device;

        std::vector<rdobject> objects;
        std::vector<std::string> source_lines;
        std::vector<rdstatement> statements;
        std::vector<rdoperation*> operations;
        i32 start = 0;
};

#endif
