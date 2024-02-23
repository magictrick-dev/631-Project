#include <windows.h>

#include <core.h>
#include <window.h>
#include <renderer/renderer.h>

typedef void (*set_in_window_fptr)(i32, i32, HDC, COLORREF&);

static inline void
set_in_window(i32 x, i32 y, HDC device, COLORREF& ref)
{
    SetPixel(device, x, y, ref);
}

static inline void
set_in_window_noop(i32 x, i32 y, HDC device, COLORREF& ref)
{
    return;
}

void
set_pixel(i32 x, i32 y, v3 color)
{

    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    COLORREF cref = RGB(red, green, blue);

    window_state *window = get_window_state();

    // Rather than using branching, we can actually use some simple arithmetic
    // to handle out-of-bounds. That way we don't have an issue with branch prediction
    // on each call.
    //
    // TODO(Chris): This is a silly way to do this, faster than the if-statement,
    //              slower than just changing the bounds of the line algorithm
    //              at invocation time since we still operate on pixels not
    //              bounded to the device. Oh well, modern hardware is kinda fast.
    i32 rb = window->width - x;
    i32 bb = window->height - y;

    u32 bounds_left = (0x1 << 31) & x;
    u32 bounds_top = (0x1 << 31) & y;
    u32 bounds_right = (0x1 << 31) & rb;
    u32 bounds_bottom = (0x1 << 31) & bb;
    u32 bounded = ((bounds_left | bounds_top | bounds_right | bounds_bottom) >> 31);

    static set_in_window_fptr setfps[2] { set_in_window, set_in_window_noop };
    setfps[bounded](x, y, window->device, cref);

}

void
set_fill(v3 color)
{

    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    COLORREF cref = RGB(red, green, blue);

    BITMAPINFO info = {};
    info.bmiHeader.biSize           = sizeof(BITMAPINFO);
    info.bmiHeader.biWidth          = 1;
    info.bmiHeader.biHeight         = 1;
    info.bmiHeader.biPlanes         = 1;
    info.bmiHeader.biBitCount       = 32;
    info.bmiHeader.biCompression    = 0;
    info.bmiHeader.biSizeImage      = 0;

    u32 fill_pixel = (u32)cref;
    window_state *window = get_window_state();

    int ret = StretchDIBits(window->device, 0, 0, window->width, window->height,
            0, 0, 1, 1, &fill_pixel, &info, DIB_RGB_COLORS, SRCCOPY);

}

void
set_line(i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color)
{

    // Brehsenham's line algorithm.

    i32 dx = x2 - x1;
    i32 dy = y2 - y1;

    if (abs(dy) < abs(dx))
    {
        i32 p0 = 2 * abs(dy) - abs(dx);
        i32 i1 = 2 * abs(dy) - 2 * abs(dx);
        i32 i2 = 2 * abs(dy);

        i32 y = y1;
        i32 p = p0;

        i32 sub = 1;
        if (dy < 0) sub = -1;
        
        if (x1 < x2)
        {
            // X1 < X2 (left to right)
            for (i32 x = x1; x < x2; ++x)
            {
                set_pixel(x, y, color);
                if (p > 0)
                {
                    y += sub;
                    p += i1;
                }
                else
                {
                    p += i2;
                }
            }
        }

        else
        {
            // X1 < X2 (left to right)
            for (i32 x = x1; x >= x2; --x)
            {
                set_pixel(x, y, color);
                if (p > 0)
                {
                    y += sub;
                    p += i1;
                }
                else
                {
                    p += i2;
                }
            }
        }

    }  

    else
    {

        i32 p0 = 2 * abs(dx) - abs(dy);
        i32 i1 = 2 * abs(dx) - 2 * abs(dy);
        i32 i2 = 2 * abs(dx);

        i32 x = x1;
        i32 p = p0;

        i32 sub = 1;
        if (dx < 0) sub = -1;
        
        if (y1 < y2)
        {
            // X1 < X2 (left to right)
            for (i32 y = y1; y < y2; ++y)
            {
                set_pixel(x, y, color);
                if (p > 0)
                {
                    x += sub;
                    p += i1;
                }
                else
                {
                    p += i2;
                }
            }
        }

        else
        {
            // X1 < X2 (left to right)
            for (i32 y = y1; y >= y2; --y)
            {
                set_pixel(x, y, color);
                if (p > 0)
                {
                    x += sub;
                    p += i1;
                }
                else
                {
                    p += i2;
                }
            }
        }

    }


}
