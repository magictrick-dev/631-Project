#include <windows.h>

#include <iostream>

#include <core.h>
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

// --- Genereal Set Fill -------------------------------------------------------
void
set_fill(renderable_device *device, v3 color)
{
    device->set_fill(color);
}

// --- General Point Set -------------------------------------------------------
void
set_point(renderable_device *device, i32 x, i32 y, i32 z, v3 color)
{
    device->set_pixel(x, y, z, color);
}

// --- Bresenham's Line Algorithm ----------------------------------------------
//
// This absolute monstrocity is the result of taking really poor notes.
//

void
set_line(renderable_device *device, i32 x1, i32 x2, i32 y1, i32 y2, i32 z1, i32 z2, v3 color)
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
                device->set_pixel(x, y, z1, color);
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
                device->set_pixel(x, y, z1, color);
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
                device->set_pixel(x, y, z1, color);
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
                device->set_pixel(x, y, z1, color);
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
set_circle_pixels(renderable_device *device, i32 x, i32 y, i32 xo, i32 yo, i32 z, v3 color)
{
    device->set_pixel(xo + x, yo + y, z, color);
    device->set_pixel(xo - x, yo + y, z, color);
    device->set_pixel(xo + x, yo - y, z, color);
    device->set_pixel(xo - x, yo - y, z, color);

    device->set_pixel(xo + y, yo + x, z, color);
    device->set_pixel(xo - y, yo + x, z, color);
    device->set_pixel(xo + y, yo - x, z, color);
    device->set_pixel(xo - y, yo - x, z, color);
}

void
set_circle(renderable_device *device, i32 x_offset, i32 y_offset, i32 z_offset, i32 radius, v3 color)
{

    i32 x = 0;
    i32 y = radius;
    i32 p = 1 - radius;

    while (x <= y)
    {

        set_circle_pixels(device, x, y, x_offset, y_offset, z_offset, color);
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
find_span(renderable_device *device, i32 *x_start, i32 *x_end, i32 y, u32 seed)
{

    
    u32 current_pixel = device->get_pixel(*x_start, y, 0);
    if (current_pixel != seed)
        return false;
    
    *x_end = *x_start;

    while (current_pixel == seed && *x_start >= 0)
    {
        current_pixel = device->get_pixel(--(*x_start), y, 0);
    }

    (*x_start)++;

    current_pixel = device->get_pixel(*x_end, y, 0);
    while (current_pixel == seed && *x_end <= device->width)
    {
        current_pixel = device->get_pixel(++(*x_end), y, 0);
    }

    return true;

}

inline static void
fill_span(renderable_device *device, i32 x_start, i32 x_end, i32 y, v3 color)
{

    for (i32 x = x_start; x < x_end; ++x)
        device->set_pixel(x, y, 0, color);

}

inline static void
fff4(renderable_device *device, i32 x_start, i32 x_end, i32 y, u32 seed, v3 color)
{

    if (x_start >= device->width)
        return;
    if (x_end < 0)
        return;
    if (y < 0)
        return;
    if (y >= device->height)
        return;

    if (find_span(device, &x_start, &x_end, y, seed))
    {
        fill_span(device, x_start, x_end, y, color);

        i32 new_xs = x_start;
        i32 new_xe = x_start;
        for (; new_xs < x_end; new_xs = new_xe)
        {
            if (y + 1 >= device->height)
                return;
            if (find_span(device, &new_xs, &new_xe, y + 1, seed))
            {
                fff4(device, new_xs, new_xe, y + 1, seed, color);
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
            if (find_span(device, &new_xs, &new_xe, y - 1, seed))
            {
                fff4(device, new_xs, new_xe, y - 1, seed, color);
            }
            else
            {
                new_xe++;
            }
        }

    }
}

void
set_flood(renderable_device *device, i32 x, i32 y, i32 z, v3 color)
{

    u32 seed = device->get_pixel(x, y, z);
    fff4(device, x, x, y, seed, color);

}

