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

    int dx = (int)b.x - (int)a.x;
    int dy = (int)b.y - (int)a.y;

    int n_steps = max(abs(dx), abs(dy));
    
    for (size_t n = 0; n <= n_steps; ++n)
    {
        f32 t = (f32)n / n_steps;
        //v4 p = v1.position + (t * (v2.position - v2.position));
 
        v4 p = a + (t * (b - a));

        if (set_depthbuffer(p.x, p.y, p.z))
            device->set_pixel(p.x, p.y, p.z, draw_color);       
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

// --- Line Clipping -----------------------------------------------------------

bool
line_clip(v4 *a, v4 *b)
{

    v4& point_a = *a;
    v4& point_b = *b;

    f32 bc0[6] = {
        point_a.x,
        point_a.w - point_a.x,
        point_a.y,
        point_a.w - point_a.y,
        point_a.z,
        point_a.w - point_a.z,
    };

    f32 bc1[6] = {
        point_b.x,
        point_b.w - point_b.x,
        point_b.y,
        point_b.w - point_b.y,
        point_b.z,
        point_b.w - point_b.z,
    };

    u32 kode0 = 0;
    u32 kode1 = 0;
    for (size_t i = 0; i < 6; ++i)
    {
        kode0 |= (bc0[i] <= 0.0f) << i;
        kode1 |= (bc1[i] <= 0.0f) << i;
    }

    u32 kode_overlap = kode0 & kode1;
    u32 kode_union = kode0 | kode1;
    
    if (kode_overlap)
    {
        return false;
    }
    else if (!kode_union)
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
        
        f32 alpha = bc0[i] / (bc0[i] - bc1[i]);

        u32 mask = 0x1 << i;
        if (kode0 & mask)
        {
            alpha0 = max(alpha, alpha0);
        }
        else
        {
            alpha1 = min(alpha, alpha1);
        }

        if (alpha1 <= alpha0)
            return false;
    }

    v4 final_a = point_a;
    v4 final_b = point_b;
    final_a = point_a + (alpha0 * (point_b - point_a));
    final_b = point_a + (alpha1 * (point_b - point_a));

    for (size_t i = 0; i < 4; ++i)
    {
        final_a.elements[i] = point_a.elements[i] +
            (alpha0 * (point_b.elements[i] - point_a.elements[i]));
        final_b.elements[i] = point_a.elements[i] +
            (alpha1 * (point_b.elements[i] - point_a.elements[i]));
    }
    
    *a = final_a;
    *b = final_b;

    return true;

}

// --- Depth Buffering ---------------------------------------------------------

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

    clear_depthbuffer();

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
        for (size_t idx = 0; idx < (depth_buffer_width * depth_buffer_height); ++idx)
            depth_buffer[idx] = 1.0f;
    }
}

bool
set_depthbuffer(u32 width, u32 height, f32 value)
{

    f32 *db = get_depthbuffer();
    assert(db != NULL);

    f32 *current_z = &db[height * depth_buffer_width + width];
    //std::cout << *current_z << " " << value << std::endl;
    if (value <= *current_z)
    {
        *current_z = value;
        return true;
    }

    return false;

}

// --- Polygon Clipping --------------------------------------------------------
//
// REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
//

#define POLY_CLIP_LEFT      0
#define POLY_CLIP_RIGHT     1
#define POLY_CLIP_BOTTOM    2
#define POLY_CLIP_TOP       3
#define POLY_CLIP_BACK      4
#define POLY_CLIP_FRONT     5

static attr_point               first_seen[6];
static attr_point               last_seen[6];
static bool                     first_init[6];
static std::vector<attr_point>  clip_list;

static void         clip_point(attr_point p, i32 stage);
static attr_point   clip_intersect(attr_point a, attr_point b, i32 stage);
static bool         clip_in_boundary(attr_point p, i32 stage);
static bool         clip_is_crossing(attr_point a, attr_point b, i32 stage);

i32
poly_clip(std::vector<attr_point>& poly_list, std::vector<attr_point>& out)
{
    
    // Initialize.
    clip_list.clear();
    for (size_t idx = 0; idx < 6; ++idx) first_init[idx] = false;

#if 0
    for (size_t idx = 0; idx < poly_list.size(); ++idx)
    {
        poly_list[idx].position.x = poly_list[idx].position.x / poly_list[idx].position.w;
        poly_list[idx].position.y = poly_list[idx].position.y / poly_list[idx].position.w;
        poly_list[idx].position.z = poly_list[idx].position.z / poly_list[idx].position.w;
        poly_list[idx].position.w = poly_list[idx].position.w / poly_list[idx].position.w;
    }
#endif

        
    // We go in deep.
    for (size_t idx = 0; idx < poly_list.size(); ++idx)
        clip_point(poly_list[idx], POLY_CLIP_LEFT);

    // Final stage.
    for (size_t idx = 0; idx < 6; ++idx)
    {
        if (first_init[idx] && clip_is_crossing(last_seen[idx], first_seen[idx], idx))
        {
            attr_point np = clip_intersect(last_seen[idx], first_seen[idx], idx);
            if (idx == 5)
                clip_list.push_back(np);
            else
                clip_point(np, idx+1);
        }
    }

    out = clip_list;

#if 0
    std::cout << "\nClip output:" << std::endl;
    for (auto a : out)
    {
        v4 t = a.position;
        t.x = t.x / t.w;
        t.y = t.y / t.w;
        t.z = t.z / t.w;
        t.w = t.w / t.w;
        std::cout << t << std::endl;
    }
#endif

    return out.size();
}

static void
clip_point(attr_point p, i32 stage)
{
    
    // Is this the first time we've seen a point?
    if (first_init[stage] == false)
    {
        first_init[stage] = true;
        first_seen[stage] = p;
    }

    // Previous point exists.
    else
    {
        if (clip_is_crossing(p, last_seen[stage], stage))
        {
            attr_point np = clip_intersect(p, last_seen[stage], stage);
            if (stage == 5)
                clip_list.push_back(np);
            else
                clip_point(np, stage+1);
        }
    }

    last_seen[stage] = p;

    // Check if it is in boundary.
    if (clip_in_boundary(p, stage))
    {
        if (stage == 5)
            clip_list.push_back(p);
        else
            clip_point(p, stage+1);
    }

}

static attr_point
clip_intersect(attr_point a, attr_point b, i32 stage)
{

    double alpha_value = 0.0f;
 
    v4 point_a = a.position;
    v4 point_b = b.position;

    double ac[6] = {
        point_a.x,
        point_a.w - point_a.x,
        point_a.y,
        point_a.w - point_a.y,
        point_a.z,
        point_a.w - point_a.z,
    };

    double bc[6] = {
        point_b.x,
        point_b.w - point_b.x,
        point_b.y,
        point_b.w - point_b.y,
        point_b.z,
        point_b.w - point_b.z,

    };

    switch(stage)
    {
        case POLY_CLIP_LEFT:
        {
            //alpha_value = a.position.x / (a.position.x - b.position.x);
            alpha_value = ac[0] / (ac[0] - bc[0]);
            break;
        };
        case POLY_CLIP_RIGHT:
        {
            alpha_value = ac[1] / (ac[1] - bc[1]);
            break;
        };
        case POLY_CLIP_TOP:
        {
            alpha_value = ac[2] / (ac[2] - bc[2]);
            break;
        };
        case POLY_CLIP_BOTTOM:
        {
            alpha_value = ac[3] / (ac[3] - bc[3]);
            break;
        };
        case POLY_CLIP_BACK:
        {
            alpha_value = ac[4] / (ac[4] - bc[4]);
            break;
        };
        case POLY_CLIP_FRONT:
        {
            alpha_value = ac[5] / (ac[5] - bc[5]);
            break;
        };
    };

    //alpha_value = ac / (ac - bc);
    attr_point result = {};
    for (size_t i = 0; i < ATTR_SIZE; ++i)
    {
        result.coord[i] = a.coord[i] + ( alpha_value * ( b.coord[i] - a.coord[i] ));
    }
    return result;
    //return interpolate_attributed_point(a, b, alpha_value);

}

static bool
clip_in_boundary(attr_point p, i32 stage)
{
    if (stage == POLY_CLIP_LEFT)
        return !(p.position.x < 0.0f);
    else if (stage == POLY_CLIP_RIGHT)
        return !((p.position.w - p.position.x) < 0.0f);
    else if (stage == POLY_CLIP_TOP)
        return !((p.position.y) < 0.0f);
    else if (stage == POLY_CLIP_BOTTOM)
        return !((p.position.w - p.position.y) < 0.0f);
    else if (stage == POLY_CLIP_BACK)
        return !((p.position.z) < 0.0f);
    else
        return !((p.position.w - p.position.z) < 0.0f);


#if 0
    switch(stage)
    {
        case POLY_CLIP_LEFT:
        {
            if (p.position.x >= 0.0f) return true;           
            else return false;
            break;
        };
        case POLY_CLIP_RIGHT:
        {
            if ((p.position.w - p.position.x) <= 1.0f) return true;
            else return false;
            break;
        };
        case POLY_CLIP_TOP:
        {
            if (p.position.y >= 0.0f) return true;
            else return false;
            break;
        };
        case POLY_CLIP_BOTTOM:
        {
            if ((p.position.w - p.position.y) <= 1.0f) return true;
            else return false;
            break;
        };
        case POLY_CLIP_FRONT:
        {
            if (p.position.z >= 0.0f) return true;
            else return false;
            break;
        };
        case POLY_CLIP_BACK:
        {
            if ((p.position.w - p.position.z) <= 1.0f) return true;
            else return false;
            break;
        };
        default:
        {
            return false;
        };  
    };
#endif

}

static bool
clip_is_crossing(attr_point a, attr_point b, i32 stage)
{
    return (clip_in_boundary(a, stage) != clip_in_boundary(b, stage));
}

// --- Scan Conversion ---------------------------------------------------------

struct edge
{
    int y_last;

    attr_point p;
    attr_point inc;

    edge *next;
};

static edge*    edge_table      = NULL;
static u32      edge_table_size = 0;

void
create_edgetable(u32 height)
{
    if (edge_table != NULL)
    {
        free(edge_table);
    }

    edge_table = (edge*)malloc(sizeof(edge) * height);
    edge_table_size = height;

    for (size_t i = 0; i < edge_table_size; ++i)
        edge_table[i].next = NULL;

}

void
insert_edge(edge* active, edge* e)
{
    edge *p, *q = active;
    p = q->next;
    while (p != NULL && (e->p.position.x > p->p.position.x))
    {
        q = p;
        p = p->next;
    }
    e->next = q->next;
    q->next = e;
}

void
make_edge_record(attr_point lower, attr_point upper)
{

    // Calculates dy.
    f32 dy = upper.position.y - lower.position.y;

    // Create edge.
    edge* instance = (edge*)malloc(sizeof(edge));
    *instance = {};
    instance->next = NULL;
    
    // Store edge increment.
    attr_point einc;
    for (size_t i = 0; i < ATTR_SIZE; ++i)
    {
        einc.coord[i] = (upper.coord[i] - lower.coord[i]) / dy;
    }

    instance->inc = einc;

    // Edge start.
    f32 factor = ceil(lower.position.y) - lower.position.y;

    attr_point p;
    for (size_t i = 0; i < ATTR_SIZE; ++i)
    {
        p.coord[i] = lower.coord[i] + factor * instance->inc.coord[i];
    }
    instance->p = p;

    instance->y_last = ceil(upper.position.y) - 1;

    f32 loc = ceil(lower.position.y);
    insert_edge(&edge_table[(i32)loc], instance);

}

bool
build_edge_list(std::vector<attr_point>& clip_list)
{
    
    int v1, v2;
    bool scanline_cross = false;

    v1 = clip_list.size() - 1;
    for (v2 = 0; v2 < clip_list.size(); ++v2)
    {
        if (ceil(clip_list[v1].position.y) != ceil(clip_list[v2].position.y))
        {
            scanline_cross = true;
            if (clip_list[v1].position.y < clip_list[v2].position.y)
            {
                make_edge_record(clip_list[v1], clip_list[v2]);
            }
            else
            {
                make_edge_record(clip_list[v2], clip_list[v1]);
            }
        }
        
        v1 = v2;

    }

    return scanline_cross;

}

void
add_active_list(edge* active, i32 scan_line)
{
    edge *p, *q;

    p = edge_table[scan_line].next;

    while (p)
    {
        q = p->next;
        insert_edge(active, p);
        p = q;
    }

    edge_table[scan_line].next = NULL;

}

void
fill_edge_pairs(renderable_device *device, edge* active, i32 scan, 
        v3 color, light_model& lighting)
{

    edge *p1, *p2;
    p1 = active->next;
    while (p1)
    {
        p2 = p1->next;
        if (p1->p.position.x != p2->p.position.x)
        {
            f32 dx = p2->p.position.x - p1->p.position.x;
            attr_point inc;
            for (size_t i = 0; i < ATTR_SIZE; ++i)
            {
                inc.coord[i] = (p2->p.coord[i] - p1->p.coord[i]) / dx;
            }
            
            f32 factor = ceil(p1->p.position.x) - p1->p.position.x;
            
            attr_point value;
            for (size_t i = 0; i < ATTR_SIZE; ++i)
            {
                value.coord[i] = p1->p.coord[i] + factor * inc.coord[i];
            }

            f32 endx = ceil(p2->p.position.x);

            while (value.position.x < endx)
            {
                
                v4 res = value.position;
                res.y = scan;

                // Set surface_point_value and then normalize with constant.
                lighting.surface_point_values = value;
                for (size_t i = 0; i < 16; ++i)
                {
                    lighting.surface_point_values.coord[i] /= value.constant;
                }

                v3 fragment_color = {};
                lighting.shader_function(fragment_color, lighting);

                if (set_depthbuffer(res.x, scan, res.z))
                    device->set_pixel(res.x, scan, res.z, fragment_color);

                for (size_t i = 0; i < ATTR_SIZE; ++i)
                {
                    value.coord[i] += inc.coord[i];
                }
            }

        }

        p1 = p2->next;
    }

}

void
delete_after(edge* q)
{

    edge *p = q->next;
    q->next = p->next;
    free(p);

}

void
update_active(edge* active, i32 scan)
{
    edge *q = active, *p = active->next;
    while (p)
    {
        if (scan == p->y_last)
        {
            p = p->next;
            delete_after(q);
        }
        else
        {
            
            for (size_t i = 0; i < ATTR_SIZE; ++i)
            {
                p->p.coord[i] += p->inc.coord[i];
            }

            q = p;
            p = p->next;

        }
    }
}

void
resort_active(edge *active)
{
    edge *q, *p = active->next;
    active->next = NULL;
    while (p)
    {
        q = p->next;
        insert_edge(active, p);
        p = q;
    }
}

void
scan_convert(renderable_device *device, std::vector<attr_point>& clip_list,
        v3 color, light_model& lighting)
{

    assert(edge_table != NULL);

    edge aet_head = {};
    aet_head.next = NULL;

    if (!build_edge_list(clip_list))
        return;



    for (int scan = 0; scan < edge_table_size; ++scan)
    {
        
        add_active_list(&aet_head, scan);

        if (aet_head.next != NULL)
        {
            fill_edge_pairs(device, &aet_head, scan, color, lighting);
            update_active(&aet_head, scan);
            resort_active(&aet_head);
        }

    }

    return;
}

// --- Lighting ----------------------------------------------------------------

f32 clamp(f32 min, f32 max, f32 value)
{
    if (value <= min)
        return min;
    else if (value >= max)
        return max;
    else
        return value;
}

v3 clampv(f32 min, f32 max, v3 v)
{
    if (v.r < min) v.r = min;
    if (v.g < min) v.g = min;
    if (v.b < min) v.b = min;
    if (v.r > max) v.r = max;
    if (v.g > max) v.g = max;
    if (v.b > max) v.b = max;
    return v;
}

void clamp_negatives(v3& v)
{
    if (v.x < 0.0f) v.x = 0.0f;
    if (v.y < 0.0f) v.y = 0.0f;
    if (v.z < 0.0f) v.z = 0.0f;
}

v3
diffuse(v3 Cs, light_model& model)
{

    v3 N = {};
    if (model.vertex_normal_flag == false || model.vertex_interpolate_flag == false)
        N = model.poly_normal.xyz;
    else
        N = model.surface_point_values.normals;
    N = normalize(N);

    v3 color_out = { 0.0f, 0.0f, 0.0f };
    for (size_t i = 0; i < model.farlights.size(); ++i)
    {

        v3 L = normalize(model.farlights[i].L);
        f32 NL = clamp(0.0f, 1.0f, dot(N, L));

        if (NL < 0.0f) continue;

        color_out.r += NL * model.farlights[i].C.r * Cs.r * model.diffuse_coefficient;
        color_out.g += NL * model.farlights[i].C.g * Cs.g * model.diffuse_coefficient;
        color_out.b += NL * model.farlights[i].C.b * Cs.b * model.diffuse_coefficient;

    }

    for (size_t i = 0; i < model.pointlights.size(); ++i)
    {
        v3 Li   = model.pointlights[i].L - model.surface_point_values.world;

        f32 Di  = 1.0f / magnitude_squared(Li);
        v3 Ci   = model.pointlights[i].C;
        f32 NL  = dot(N, normalize(Li));
        f32 NLc = clamp(0.0f, 1.0f, NL);
        if (NLc <= 0.0f) continue;

        color_out.r += Ci.r * NLc * Cs.r * model.diffuse_coefficient * Di;
        color_out.g += Ci.g * NLc * Cs.g * model.diffuse_coefficient * Di;
        color_out.b += Ci.b * NLc * Cs.b * model.diffuse_coefficient * Di;


    }

    return color_out;

}

v3
specular(v3 Cs, light_model& model)
{

    // Reflectance Function:
    //
    //      Ks Ci E(Ii * V.R)^ne
    //
    //      R = (2N.L/||N||^2)N - L
    //

    v3 N = {};
    if (model.vertex_normal_flag == false || model.vertex_interpolate_flag == false)
        N = model.poly_normal.xyz;
    else
        N = model.surface_point_values.normals;

    v3 V = model.camera_eye - model.surface_point_values.world;

    v3 color_out = { 0.0f, 0.0f, 0.0f };
    for (size_t i = 0; i < model.farlights.size(); ++i)
    {

        v3 L    = normalize(model.farlights[i].L);
        f32 NL  = clamp(0.0f, 1.0f, dot(normalize(N), normalize(L)));
        
        v3 R    = (2 * NL / magnitude_squared(N)) * N - L;
        f32 VR  = clamp(0.0f, 1.0f, dot(normalize(V), normalize(R)));

        if (NL <= 0.0f) continue;
        if (VR <= 0.0f) continue;

        f32 Ks = model.specular_coefficient;
        v3 Cspec = model.specular_color;
        v3 Is = model.farlights[i].C;
        f32 Ne = model.specular_exponent;

        color_out.r += Ks * Cs.r * powf(VR, Ne) * Is.r;
        color_out.g += Ks * Cs.g * powf(VR, Ne) * Is.g;
        color_out.b += Ks * Cs.b * powf(VR, Ne) * Is.b;

    }

#if 1
    for (size_t i = 0; i < model.pointlights.size(); ++i)
    {

        v3 L    = model.pointlights[i].L - model.surface_point_values.world;
        f32 Di  = 1.0f / magnitude_squared(L);
        f32 NL  = clamp(0.0f, 1.0f, dot(normalize(N), normalize(L)));
        
        v3 R    = (2 * NL / magnitude_squared(N)) * N - L;
        f32 VR  = clamp(0.0f, 1.0f, dot(normalize(V), normalize(R)));

        if (NL <= 0.0f) continue;
        if (VR <= 0.0f) continue;

        f32 Ks = model.specular_coefficient;
        v3 Cspec = model.specular_color;
        v3 Is = model.pointlights[i].C;
        f32 Ne = model.specular_exponent;

        color_out.r += Ks * Cs.r * Di * powf(VR, Ne) * Is.r;
        color_out.g += Ks * Cs.g * Di * powf(VR, Ne) * Is.g;
        color_out.b += Ks * Cs.b * Di * powf(VR, Ne) * Is.b;

    }
#endif

    return color_out;



}

v3
ambient(v3 Cs, light_model& model)
{

    v3 color;
    color.r = model.ambient_coefficient * Cs.r * model.ambient.C.r;
    color.g = model.ambient_coefficient * Cs.g * model.ambient.C.g;
    color.b = model.ambient_coefficient * Cs.b * model.ambient.C.b;

    return color;

}

void
matte_shader(v3& color, light_model& model)
{

    v3 Cs = {};
    if (model.vertex_color_flag == false)
        Cs = model.surface_color;
    else
        Cs = model.surface_point_values.color;

    v3 ambient_component = ambient(Cs, model);
    v3 diffuse_component = diffuse(Cs, model);

    color = clampv(0.0f, 1.0f, ambient_component + diffuse_component);

}

void
metal_shader(v3& color, light_model& model)
{
    v3 Cs = {};
    if (model.vertex_color_flag == false)
        Cs = model.surface_color;
    else
        Cs = model.surface_point_values.color;

    v3 ambient_component = ambient(Cs, model);
    v3 diffuse_component = diffuse(Cs, model);
    v3 specular_component = specular(Cs, model);

    color = clampv(0.0f, 1.0f, ambient_component + specular_component);

}

void
plastic_shader(v3& color, light_model& model)
{
    v3 Cs = {};
    if (model.vertex_color_flag == false)
        Cs = model.surface_color;
    else
        Cs = model.surface_point_values.color;

    v3 ambient_component = ambient(Cs, model);
    v3 diffuse_component = diffuse(Cs, model);
    v3 specular_component = specular(Cs, model);

    color = clampv(0.0f, 1.0f, ambient_component + diffuse_component + specular_component);

}
