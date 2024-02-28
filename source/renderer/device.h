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

        bool    init_window(bool double_buffer = false);
        bool    should_close();
        void    swap_buffers();

        // Some windows shite we need to keep track of.
        HDC     device_context  = NULL;
        HWND    window_handle   = NULL;
        bool    running         = true;

        // For rendering.
        bool    double_buffered = false;
        u32    *front_buffer    = NULL;
        u32    *back_buffer     = NULL;
        size_t  buffer_width    = 0;
        size_t  buffer_height   = 0;
        
        // Our window lives on a seperate thread
        // and observes the changes we make to the pixel buffer it is blitting.
        DWORD   window_thread_id    = NULL;
        HANDLE  window_thread_hndl  = NULL;
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
