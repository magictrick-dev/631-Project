#ifndef J5_RDVIEW_PARSER_H
#define J5_RDVIEW_PARSER_H
#include <core.h>

#define RDVIEW_DEVICE_WINDOW     0
#define RDVIEW_DEVICE_PNM        1
#define RDVIEW_DEVICE_BMP        2

#define RDVIEW_OPTYPE_ROOT       0
#define RDVIEW_OPTYPE_COLOR      1
#define RDVIEW_OPTYPE_CANVAS     2
#define RDVIEW_OPTYPE_POINT      9
#define RDVIEW_OPTYPE_LINE      10

#define RDVIEW_DRAW_TYPE_OUTPUT  0
#define RDVIEW_DRAW_TYPE_SINGLE  0
#define RDVIEW_DRAW_TYPE_DOUBLE  1
#define RDVIEW_DRAW_TYPE_STEP    2

struct rdview_operation
{
    u32     type;
    void   *operands;

    rdview_operation *next_operation;
};

struct rdview_color
{
    f32 r;
    f32 g;
    f32 b;
};

struct rdview_point
{
    i32 x;
    i32 y;
    i32 z;
};

struct rdview_line
{
    i32 x1;
    i32 y1;
    i32 z1;
    i32 x2;
    i32 y2;
    i32 z2;
};

struct rdview_configuration
{

    struct
    {
        char    display_name[80];
        
        u32     device_draw_method;
        u32     device_type;
        i32     device_width;
        i32     device_height;
    } state;

    v3 draw_color;
    v3 canvas_color;

    rdview_operation *operation_list;

};

char*   rdview_source_fetch(const char *file_path);
void    rdview_source_free(char *buffer);
bool    rdview_source_parse(rdview_configuration *config, const char *buffer);
void    rdview_source_run(rdview_configuration *config);

class rdview_operation
{
    public:
        virtual void run(void) = NULL;
};

class rdview_point : public rdview_operation
{
    public:
        inline rdview_point(v3 location);

    public:
        inline virtual void run(void);

    protected:
        v3 _where;
};

rdview_point::
rdview_point(v3 location)
{
    this->_where = location;
}

inline void rdview_point::
run(void)
{
    
}

#endif
