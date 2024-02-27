#include <renderer/device.h>
#include <fstream>

// --- Window Device -----------------------------------------------------------

LRESULT CALLBACK
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

u32 window_device::
get_pixel(i32 x, i32 y, i32 z)
{
    COLORREF pixel = GetPixel(this->device_context, x, y);
    return (u32)pixel;
}

window_device::
window_device(std::string title, i32 width, i32 height)
{

    this->width = width;
    this->height = height;
    this->title = title;

}

bool window_device::
should_close()
{

    // Peek the window.
    MSG current_message = {};
    while (PeekMessageW(&current_message, 0, 0, 0, PM_REMOVE))
    {

        if (current_message.message == WM_QUIT)
        {
            this->running = false;
            return true;
        }

        TranslateMessage(&current_message);
        DispatchMessageW(&current_message);

    }

    return false;

}

bool window_device::
init_window()
{

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
    //int request_width = current_configuration.state.device_width;
    //int request_height = current_configuration.state.device_height;
    int request_width = this->width;
    int request_height = this->height;

    RECT window_rect = {};
    window_rect.right = request_width;
    window_rect.bottom = request_height;
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

    this->window_handle = window_handle;
    this->device_context = GetDC(window_handle);
    
    // Change the window title.
    SetWindowTextA(window_handle, this->title.c_str());

    // Show the window.
    ShowWindow(window_handle, SW_SHOWNORMAL);


    return true;
}


void window_device::
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

    int ret = StretchDIBits(this->device_context, 0, 0, this->width, this->height,
            0, 0, 1, 1, &fill_pixel, &info, DIB_RGB_COLORS, SRCCOPY);
}

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

void window_device::
set_pixel(i32 x, i32 y, i32 z, v3 color)
{

    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    COLORREF cref = RGB(red, green, blue);

    // Rather than using branching, we can actually use some simple arithmetic
    // to handle out-of-bounds. That way we don't have an issue with branch prediction
    // on each call.
    //
    // TODO(Chris): This is a silly way to do this, faster than the if-statement,
    //              slower than just changing the bounds of the line algorithm
    //              at invocation time since we still operate on pixels not
    //              bounded to the device. Oh well, modern hardware is kinda fast.
    i32 rb = this->width - x;
    i32 bb = this->height - y;

    u32 bounds_left = (0x1 << 31) & x;
    u32 bounds_top = (0x1 << 31) & y;
    u32 bounds_right = (0x1 << 31) & rb;
    u32 bounds_bottom = (0x1 << 31) & bb;
    u32 bounded = ((bounds_left | bounds_top | bounds_right | bounds_bottom) >> 31);

    static set_in_window_fptr setfps[2] { set_in_window, set_in_window_noop };
    setfps[bounded](x, y, this->device_context, cref);

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

typedef void (*set_in_image_fptr)(u32*, u32);

static inline void
set_in_image(u32 *spot, u32 pixel)
{
    *spot = pixel;
}

static inline void
set_in_image_noop(u32 *spot, u32 pixel)
{
    return;
}

void pnm_device::
set_pixel(i32 x, i32 y, i32 z, v3 color)
{
    
    u8 red          = (u8)(255.0f * color.r);   
    u8 green        = (u8)(255.0f * color.g);   
    u8 blue         = (u8)(255.0f * color.b);

    COLORREF cref = RGB(red, green, blue);

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

