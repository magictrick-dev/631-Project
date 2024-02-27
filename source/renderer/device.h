#ifndef RENDERER_DEVICE_H
#define RENDERER_DEVICE_H
#include <core.h>
#include <string>
#include <windows.h>

class renderable_device
{

    public:
        virtual void    set_pixel(i32 x, i32 y, i32 z, v3 color)    = NULL;
        virtual u32     get_pixel(i32 x, i32 y, i32 z)              = NULL;
        virtual void    set_fill(v3 color)                          = NULL;

        i32 width;
        i32 height;
        std::string title;
};

// --- Window Device -----------------------------------------------------------


class window_device : public renderable_device
{
    public:
        window_device(std::string title, i32 width, i32 height);

        virtual void set_pixel(i32 x, i32 y, i32 z, v3 color);
        virtual u32  get_pixel(i32 x, i32 y, i32 z);
        virtual void set_fill(v3 color);

        bool    init_window();
        bool    should_close();

    protected:
        HDC     device_context  = NULL;
        HWND    window_handle   = NULL;
        bool    running         = true;
};

// --- PNM Device --------------------------------------------------------------

class pnm_device : public renderable_device
{
    public:
        pnm_device(std::string title, i32 width, i32 height);

        virtual void set_pixel(i32 x, i32 y, i32 z, v3 color);
        virtual u32  get_pixel(i32 x, i32 y, i32 z);
        virtual void set_fill(v3 color);

        bool init_image();
        bool save_image();

        u8* pixel_buffer;
};

#endif
