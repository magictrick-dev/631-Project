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

#include <rdview/operations.h>
#include <rdview/statement.h>
#include <renderer/device.h>

void        rdview_parser_strip_whitespace(std::string &line);
void        rdview_parser_rejoin_strings(std::vector<std::string>& lk_line);
std::string rdview_parser_keyword_dequote(std::string keyword);

// --- RDView Parser & Configuration -------------------------------------------

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

    public:
        i32 width        = 640;
        i32 height       = 480;
        v3  draw_color   = { 1.0f, 1.0f, 1.0f };
        v3  canvas_color = { 0.0f, 0.0f, 0.0f };

        std::string title;
        u32         device;
        u32         mode;

        renderable_device *active_device;

        std::vector<std::string> source_lines;
        std::vector<rdstatement> statements;
        std::vector<rdoperation*> operations;
        i32 start = 0;
};

#endif
