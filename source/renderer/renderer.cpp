#include <windows.h>

#include <iostream>

#include <core.h>
#include <window.h>
#include <renderer/renderer.h>

// --- Device Specific Functions -----------------------------------------------
//
// set_pixel()
//      In general, you shouldn't do bounds checking on frequently executed operation
//      such as this, but one way to ensure that we aren't murdering performance
//      through branch prediction is by numerically determining our operations.
//      It is obviously faster to constrain bounds to the device's dimensions, but
//      this method makes it less painful to do so.
//
// get_pixel()
//      Retrieves the current pixel at the (x, y) position provided as a 32-bit
//      integer. The reasoning is that conversion to float could introduce some
//      rounding errors later down the line, so we can be sure that we have the
//      most accurate value. Also, it makes comparing two pixels REALLY easy.
//
// set_fill()
//      Not to be confused with flood_fill(), set_fill() essentially overwrites
//      the entire device's pixel buffer with a single color.
//

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
get_pixel(i32 x, i32 y, u32 *color)
{

    window_state *window = get_window_state();
    COLORREF pixel = GetPixel(window->device, x, y);
    *color = (u32)pixel;

}

// --- Bresenham's Line Algorithm ----------------------------------------------
//
// This absolute monstrocity is the result of taking really poor notes.
//

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
            for (i32 x = x1; x <= x2; ++x)
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
            for (i32 y = y1; y <= y2; ++y)
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

// --- Bresenham's Circle Algorithm --------------------------------------------
//
// Surprisingly easier than the line algorithm.
//

inline static void
set_circle_pixels(i32 x, i32 y, i32 xo, i32 yo, v3 color)
{
    set_pixel(xo + x, yo + y, color);
    set_pixel(xo - x, yo + y, color);
    set_pixel(xo + x, yo - y, color);
    set_pixel(xo - x, yo - y, color);

    set_pixel(xo + y, yo + x, color);
    set_pixel(xo - y, yo + x, color);
    set_pixel(xo + y, yo - x, color);
    set_pixel(xo - y, yo - x, color);
}

void
set_circle(i32 x_offset, i32 y_offset, i32 z_offset, i32 radius, v3 color)
{

    i32 x = 0;
    i32 y = radius;
    i32 p = 1 - radius;

    while (x <= y)
    {

        set_circle_pixels(x, y, x_offset, y_offset, color);
        x++;

        if (p > 0)
        {
            y--;
            p = p + 2 * x + 1 - 2 * y;
        }
        else
        {
            p += (2 * x) + 1;
        }

    }

}

// --- Fast Flood Fill Algorithm -----------------------------------------------
//
// Fills a region at (x, y) with a given color by selected the current (x, y)'s
// color as the seed color to find boundaries.
//

inline static bool
find_span(i32 *x_start, i32 *x_end, i32 y, u32 seed)
{

    window_state *window = get_window_state();
    
    u32 current_pixel = 0;
    get_pixel(*x_start, y, &current_pixel);
    if (current_pixel != seed)
        return false;
    
    *x_end = *x_start;

    while (current_pixel == seed && *x_start >= 0)
    {
        get_pixel(--(*x_start), y, &current_pixel);
    }

    (*x_start)++;

    get_pixel(*x_end, y, &current_pixel);
    while (current_pixel == seed && *x_end <= window->width)
    {
        get_pixel(++(*x_end), y, &current_pixel);
    }

    return true;

}

inline static void
fill_span(i32 x_start, i32 x_end, i32 y, v3 color)
{

    for (i32 x = x_start; x < x_end; ++x)
        set_pixel(x, y, color);

}

inline static void
fff4(i32 x_start, i32 x_end, i32 y, u32 seed, v3 color)
{

    window_state *window = get_window_state();
    if (x_start >= window->width)
        return;
    if (x_end < 0)
        return;
    if (y < 0)
        return;
    if (y >= window->height)
        return;

    if (find_span(&x_start, &x_end, y, seed))
    {
        fill_span(x_start, x_end, y, color);

        i32 new_xs = x_start;
        i32 new_xe = x_start;
        for (; new_xs < x_end; new_xs = new_xe)
        {
            if (y + 1 >= window->height)
                return;
            if (find_span(&new_xs, &new_xe, y + 1, seed))
            {
                fff4(new_xs, new_xe, y + 1, seed, color);
            }
            else
            {
                new_xe++;
            }
        }

        new_xs = x_start;
        new_xe = x_start;
        for (; new_xs < x_end; new_xs = new_xe)
        {
            if (y - 1 < 0)
                return;
            if (find_span(&new_xs, &new_xe, y - 1, seed))
            {
                fff4(new_xs, new_xe, y - 1, seed, color);
            }
            else
            {
                new_xe++;
            }
        }

    }
}

void
set_flood(i32 x, i32 y, i32 z, v3 color)
{

    u32 seed;
    get_pixel(x, y, &seed);
    fff4(x, x, y, seed, color);

}

