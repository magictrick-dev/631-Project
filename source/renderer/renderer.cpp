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


void
set_line_dda(renderable_device *device, v4 a, v4 b, v3 draw_color)
{

    int dx = b.x - a.x;
    int dy = b.y - a.y;

    int n_steps = max(dx, dy);
    
    for (size_t n = 0; n <= n_steps; ++n)
    {
        f32 t = (f32)n / (f32)n_steps;
        //v4 p = v1.position + (t * (v2.position - v2.position));
 
        v4 p = a + (t * (b - a));

        if (set_depthbuffer(p.x, p.y, p.z))
            device->set_pixel((int)p.x, (int)p.y, (int)p.z, draw_color);       
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

bool
line_clip(v4 *a, v4 *b)
{

    v4& point_a = *a;
    v4& point_b = *b;

    f32 ac[6] = {
        point_a.x, point_a.w - point_a.x,
        point_a.y, point_a.w - point_a.y,
        point_a.z, point_a.w - point_a.z,
    };

    f32 bc[6] = {
        point_b.x, point_b.w - point_b.x,
        point_b.y, point_b.w - point_b.y,
        point_b.z, point_b.w - point_b.z,

    };

    bool ai[6] = {
        (point_a.x < 0), ((point_a.w - point_a.x) < 0),
        (point_a.y < 0), ((point_a.w - point_a.y) < 0),
        (point_a.z < 0), ((point_a.w - point_a.z) < 0)
    };

    bool bi[6] = {
        (point_b.x < 0), ((point_b.w - point_b.x) < 0),
        (point_b.y < 0), ((point_b.w - point_b.y) < 0),
        (point_b.z < 0), ((point_b.w - point_b.z) < 0)
    };

    u32 kode0 = 0x00;
    u32 kode1 = 0x00;
    for (size_t i = 0; i < 6; ++i)
    {
        kode0 |= (ai[i]) << i;
        kode1 |= (bi[i]) << i;
    }

    u32 kode_overlap = kode0 & kode1;
    u32 kode_union = kode0 | kode1;

    // Trivial reject.
    if (kode_overlap)
    {
        return false;
    }
    
    // Trivial accept.
    else if (!(kode_union))
    {
        return true;
    }
    
    // Non-trivial check.
    f32 alpha0 = 0.0f;
    f32 alpha1 = 1.0f;
    for (size_t i = 0; i < 6; ++i)
    {
        bool intersection = ((kode_union >> i) & 0x1);

        if (!intersection)
            continue;

        f32 alpha = (ac[i]) / (ac[i] - bc[i]);

        u32 mask = 0x1 << i;
        if (kode0 & mask)
        {
            //alpha0 = (alpha <= alpha0) ? alpha0 : alpha;
            alpha0 = max(alpha0, alpha);
        }
        else
        {
            //alpha1 = (alpha <= alpha1) ? alpha : alpha1;
            alpha1 = min(alpha1, alpha);
        }

        if (alpha1 < alpha0)
            return false;

    }

    std::cout << "alpha0: " << alpha0 << " alpha1: " << alpha1 << std::endl;

    // Finally, our line is done.
    v4 final_a = point_a;
    v4 final_b = point_b;
    final_a = point_a + (alpha0 * (point_b - point_a));
    final_b = point_a + (alpha1 * (point_b - point_a));

    std::cout << "during " << final_a << " " << final_b << std::endl;

    *a = final_a;
    *b = final_b;

    return true;

}

static f32*     depth_buffer        = NULL;
static u32      depth_buffer_width  = 0;
static u32      depth_buffer_height = 0;

void
create_depthbuffer(u32 width, u32 height)
{
    if (depth_buffer != NULL)
    {
        free(depth_buffer);
    }

    depth_buffer = (f32*)malloc(sizeof(float) * width * height);
    depth_buffer_width = width;
    depth_buffer_height = height;
}

f32*
get_depthbuffer()
{

    return depth_buffer;

}

void
clear_depthbuffer()
{
    f32* db = get_depthbuffer();
    if (db != NULL)
    {
        for (size_t idx = 0; idx < depth_buffer_width * depth_buffer_height; ++idx)
            depth_buffer[idx] = 1.0f;
    }
}

bool
set_depthbuffer(u32 width, u32 height, f32 value)
{

    f32 *db = get_depthbuffer();
    assert(db != NULL);

    f32 *v = &db[height * depth_buffer_width + width];
    if (*v > value)
        return false;
    else
        *v = value;

    return true;

}
