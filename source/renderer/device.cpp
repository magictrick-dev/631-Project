#include <renderer/device.h>
#include <fstream>
#include <cassert>

typedef u32 (*get_in_image_fptr)(u32 *buffer, i32 stride, i32 x, i32 y);

static inline u32 
get_in_image(u32 *buffer, i32 stride, i32 x, i32 y)
{
    return buffer[(y * stride) + x];
}

static inline u32
get_in_image_noop(u32 *buffer, i32 stride, i32 x, i32 y)
{
    return 0xFFFFFFFF;
}

typedef void (*set_in_image_fptr)(u32*, u32);

static inline void
set_in_image(u32 *spot, u32 pixel)
{
    *spot = pixel;
#if 0
    for (int i = 0; i < 25000; ++i);
#endif
}

static inline void
set_in_image_noop(u32 *spot, u32 pixel)
{
    return;
}

#if 0
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
#endif


// --- Window Device -----------------------------------------------------------

static BOOL
window_should_close()
{

    // Peek the window.
    MSG current_message = {};
    while (PeekMessageW(&current_message, 0, 0, 0, PM_REMOVE))
    {

        if (current_message.message == WM_QUIT)
            return true;

        TranslateMessage(&current_message);
        DispatchMessageW(&current_message);

    }

    return false;

}

static LRESULT CALLBACK
window_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{

    LRESULT result = {};

    switch (message)
    {
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        };

        default:
        {
            result = DefWindowProcW(window, message, wparam, lparam);
        }
    }

    return result;

}

static DWORD WINAPI
window_thread_procedure(LPVOID user_param)
{

    // We will commit a crime with this little trick.
    window_device *dis = (window_device*)user_param;

    // Register the window.
    WNDCLASSEXW window_class        = {};
    window_class.style              = CS_OWNDC;
    window_class.cbSize             = sizeof(window_class);
    window_class.lpfnWndProc        = &window_procedure;
    window_class.hInstance          = GetModuleHandleW(NULL);
    window_class.hIcon              = LoadIconA(NULL, IDI_APPLICATION);
    window_class.hCursor            = LoadCursorA(NULL, IDC_ARROW);
    window_class.hbrBackground      = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName      = L"displayWindowClass";
    RegisterClassExW(&window_class);

    // Size the window.
    int request_width   = dis->width;
    int request_height  = dis->height;

    // Properly set the window size.
    RECT window_rect    = {};
    window_rect.right   = request_width;
    window_rect.bottom  = request_height;
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    int win_width = window_rect.right - window_rect.left;
    int win_height = window_rect.bottom - window_rect.top;

    // Create the window.
    HWND window_handle = CreateWindowExW(0, window_class.lpszClassName,
            L"CSCI 490-J5 Graphics Renderer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            win_width, win_height, 0, 0, window_class.hInstance, 0);

    // Check the window.
    if (window_handle == NULL)
    {
        printf("Unable to open a window.\n");
        return false;
    }

    // It worked.
    dis->window_handle     = window_handle;
    dis->device_context    = GetDC(window_handle);
    
    // Change the window title.
    SetWindowTextA(window_handle, dis->title.c_str());

    // Show the window.
    ShowWindow(window_handle, SW_SHOWNORMAL);

    // The big ol' spinny-boy.
    while (dis->running == true)
    {

        // Processes window messages.
        if (window_should_close())
        {
            dis->running = false;
            break;
        }

        // Now we just blit the window.
        BITMAPINFO info = {};
        info.bmiHeader.biSize           = sizeof(BITMAPINFO);
        info.bmiHeader.biWidth          = dis->buffer_width;
        info.bmiHeader.biHeight         = -dis->buffer_height;
        info.bmiHeader.biPlanes         = 1;
        info.bmiHeader.biBitCount       = 32;
        info.bmiHeader.biCompression    = 0;
        info.bmiHeader.biSizeImage      = (dis->buffer_width * dis->buffer_height * sizeof(u32));
        u32 *buffer = dis->front_buffer;

        // Basically, it just shoves the front-buffer whever it is.
        int ret = StretchDIBits(dis->device_context, 0, 0, dis->width, dis->height,
                0, 0, dis->buffer_width, dis->buffer_height, buffer, &info,
                DIB_RGB_COLORS, SRCCOPY);

    }
 
    return 0;

}

window_device::
window_device(std::string title, i32 width, i32 height)
{

    assert(width > 0);
    assert(height > 0);
    assert(!title.empty());

    // Set our basic properties.
    this->width     = width;
    this->height    = height;
    this->title     = title;

}

void window_device::
swap_buffers()
{
    u32* s = this->front_buffer;
    this->front_buffer = this->back_buffer;
    this->back_buffer = s;
    return;
}

bool window_device::
should_close()
{

    return !this->running;

}

bool window_device::
init_window(bool double_buffer)
{

    this->buffer_width = width;
    this->buffer_height = height;

    // Create our front and back buffers.
    this->front_buffer = (u32*)malloc(width * height * sizeof(u32));

    // We will always reset the swap, but sometimes the swap doesn't respect us.
    if (double_buffer)
    {
        this->double_buffered = true;
        this->back_buffer = (u32*)malloc(width * height * sizeof(u32));
    }
    else
    {
        this->back_buffer = this->front_buffer;
    }

    // Pre-set the buffers to known good values.
    for (size_t i = 0; i < (width*height); ++i)
    {

        this->front_buffer[i] = 0x00000000;
        this->back_buffer[i] = 0x00000000;

    }

    this->window_thread_hndl = CreateThread(0, 0, window_thread_procedure,
            this, 0, &this->window_thread_id);

    return true;
}

// This needs to be updated.

u32 window_device::
get_pixel(i32 x, i32 y, i32 z)
{

    i32 rb = this->width - x;
    i32 bb = this->height - y;

    u32 bounds_left = (0x1 << 31) & x;
    u32 bounds_top = (0x1 << 31) & y;
    u32 bounds_right = (0x1 << 31) & rb;
    u32 bounds_bottom = (0x1 << 31) & bb;
    u32 bounded = ((bounds_left | bounds_top | bounds_right | bounds_bottom) >> 31);

    static get_in_image_fptr getfps[2] { get_in_image, get_in_image_noop };

    // The back buffer may actually be the front buffer.
    return getfps[bounded]((u32*)this->back_buffer, (this->width), x, y);
    
}

void window_device::
set_fill(v3 color)
{

    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    u32 cref = (red << 16) | (green << 8) | (blue << 0);

    for (size_t i = 0; i < (this->width * this->height); ++i)
    {
        this->back_buffer[i] = (u32)cref;
    }

}

void window_device::
set_pixel(i32 x, i32 y, i32 z, v3 color)
{

#if 1
    if (x == 235 && y == 229)
    {
        int i = 10;
    }
#endif

    u8 red          = (u8)(255 * color.r);   
    u8 green        = (u8)(255 * color.g);   
    u8 blue         = (u8)(255 * color.b);

    //COLORREF cref = RGB(red, green, blue);

    u32 cref = (red << 16) | (green << 8) | (blue << 0);

    // Rather than using branching, we can actually use some simple arithmetic
    // to handle out-of-bounds. That way we don't have an issue with branch prediction
    // on each call.
    //
    // TODO(Chris): This is a silly way to do this, faster than the if-statement,
    //              slower than just changing the bounds of the line algorithm
    //              at invocation time since we still operate on pixels not
    //              bounded to the device. Oh well, modern hardware is kinda fast.
    i32 rb = this->width - x - 1;
    i32 bb = this->height - y - 1;

    u32 bounds_left = (0x1 << 31) & x;
    u32 bounds_top = (0x1 << 31) & y;
    u32 bounds_right = (0x1 << 31) & rb;
    u32 bounds_bottom = (0x1 << 31) & bb;
    u32 bounded = ((bounds_left | bounds_top | bounds_right | bounds_bottom) >> 31);

    //static set_in_window_fptr setfps[2] { set_in_window, set_in_window_noop };
    //setfps[bounded](x, y, this->device_context, cref);

    static set_in_image_fptr setfps[2] { set_in_image, set_in_image_noop };
    setfps[bounded](&((u32*)this->back_buffer)[(this->width * y) + x], (u32)cref);

}

// --- PNM Device --------------------------------------------------------------

pnm_device::
pnm_device(std::string title, i32 width, i32 height)
{
    this->title = title;
    this->width = width;
    this->height = height;
}

bool pnm_device::
init_image()
{
    this->pixel_buffer = (u8*)malloc(sizeof(u32) * this->width * this->height);
    return true;
}

bool pnm_device::
save_image()
{

    std::ofstream out(this->title + ".pgm", std::ios::binary);
    if (out.is_open())
    {
        out << "P6" << '\n'
            << this->width << '\n'
            << this->height << '\n'
            << 255 << '\n';
        for (size_t i = 0; i < (this->width * this->height); ++i)
        {
 
            u8 red = GetRValue(((u32*)this->pixel_buffer)[i]);
            u8 blue = GetGValue(((u32*)this->pixel_buffer)[i]);
            u8 green = GetBValue(((u32*)this->pixel_buffer)[i]);

            out << red << blue << green;

        }
    }

    out.close();
    return true;

}

void pnm_device::
set_pixel(i32 x, i32 y, i32 z, v3 color)
{
    
    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    u32 cref = (red << 16) | (green << 8) | (blue << 0);

    i32 rb = this->width - x - 1;
    i32 bb = this->height - y - 1;

    u32 bounds_left = (0x1 << 31) & x;
    u32 bounds_top = (0x1 << 31) & y;
    u32 bounds_right = (0x1 << 31) & rb;
    u32 bounds_bottom = (0x1 << 31) & bb;
    u32 bounded = ((bounds_left | bounds_top | bounds_right | bounds_bottom) >> 31);

    static set_in_image_fptr setfps[2] { set_in_image, set_in_image_noop };
    setfps[bounded](&((u32*)this->pixel_buffer)[(this->width * y) + x], (u32)cref);
    

}

u32 pnm_device::
get_pixel(i32 x, i32 y, i32 z)
{

    i32 rb = this->width - x;
    i32 bb = this->height - y;

    u32 bounds_left = (0x1 << 31) & x;
    u32 bounds_top = (0x1 << 31) & y;
    u32 bounds_right = (0x1 << 31) & rb;
    u32 bounds_bottom = (0x1 << 31) & bb;
    u32 bounded = ((bounds_left | bounds_top | bounds_right | bounds_bottom) >> 31);

    static get_in_image_fptr getfps[2] { get_in_image, get_in_image_noop };
    return getfps[bounded]((u32*)pixel_buffer, (this->width), x, y);

}

void pnm_device::
set_fill(v3 color)
{

    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    COLORREF cref = RGB(red, green, blue);

    // Performance is for nerds.
    for (size_t y = 0; y < this->height; ++y)
        for (size_t x = 0; x < this->width; ++x)
            ((u32*)pixel_buffer)[(this->width * y) + x] = (u32)cref;

}

