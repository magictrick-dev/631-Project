#include <window.h>

// --- Win32 Definitions -------------------------------------------------------
//
// Behold. Win32 code in the wild!
//

#if defined(_WIN32)
#include <windows.h>

// --- Ghost Window Definitions ------------------------------------------------
//
// We must take something that is ordinarily simple and easy to understand and
// convolute it with some dumb multi-threading logic. Thanks, Microsoft, very cool!
//
// The ghost window basically generates windows for the main thread. See the comment
// in window_create(...) for the reasoning why.
//
// In order to fix the general issue with messages being passed back to main and
// properly differentiate which window received what, we can snuggle the message
// context as a message in the message. Message-ception. We define a new message
// called UD_WINDOW_EVENT in which we define (WPARAM) as the associated pointer
// to the window state and the (LPARAM) as a pointer to message for the window.
//
// Which messages are processed, these are scanned, and then can differentiated.
//

// Our special little event functions.
#define UD_CREATE_WINDOW    (WM_USER + 0x0042)
#define UD_DESTROY_WINDOW   (WM_USER + 0x0043)

// We this to pass to the ghost thread so it can properly generate windows.
struct win32_window_creation_context
{
    DWORD       ex_style;
    LPCWSTR     class_name;
    LPCWSTR     window_name;
    DWORD       style;
    int         x;
    int         y;
    int         width;
    int         height;
    HWND        parent_window;
    HMENU       menu;
    HINSTANCE   instance;
    LPVOID      user_parameter;
};

struct win32_ghost_window
{
    HWND    window_handle;
    HANDLE  ghost_thread_sync;
    DWORD   ghost_thread_id;

    bool    ghost_thread_runtime;
    bool    ghost_thread_ready;
};


// This is the ghost window's runtime stuff.
static win32_ghost_window *ghost_window = NULL;
static DWORD main_thread_id;

static LRESULT CALLBACK
wDisplayWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

    LRESULT ret_result = 0;   

    // Essentially, our window procedure catches the messages and then forwards
    // them off as a thread message to our runtime thread, effectively making it
    // non-blocking since the message can be sent and allow the window procedure
    // to keep going.
    switch (message)
    {
        
        // For close, we simply just smuggle the window handle with w_param.
        case WM_CLOSE:
        {
            PostThreadMessageW(main_thread_id, message, (WPARAM)window, l_param);
            break;
        };

        // For all other messages that we want to process, we just send it over.
        case WM_DESTROY:
        {
            PostThreadMessageW(main_thread_id, message, w_param, l_param);
            break;
        };

        // Anything we don't care about is defaulted.
        default:
        {
            ret_result = DefWindowProcW(window, message, w_param, l_param);
        };

    };

    return ret_result;

}

static LRESULT CALLBACK
wGhostWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

    LRESULT ret_result = 0;

    switch (message)
    {
        case UD_CREATE_WINDOW:
        {

            // Cast to the creation context and then run it.
            win32_window_creation_context *context = (win32_window_creation_context*)w_param;
            HWND window_handle = CreateWindowExW(context->ex_style, context->class_name,
                    context->window_name, context->style, context->x, context->y, context->width,
                    context->height, context->parent_window, context->menu, context->instance,
                    context->user_parameter);

            // The result can be packed as a LRESULT, which is kinda neat.
            ret_result = (LRESULT)window_handle;
            break;

        };

        case UD_DESTROY_WINDOW:
        {

            // A request to destroy the window is made.
            DestroyWindow((HWND)w_param);
            break;

        };

        default:
        {

            // Generally, any other event that the ghost window receives can
            // be defaulted. We don't really care about this window's existence.
            ret_result = DefWindowProcW(window, message, w_param, l_param);
            break;

        };
    };

    return ret_result;

}

static DWORD WINAPI
ghost_window_main(LPVOID user_param)
{

    // We need to create our ghost window class.
    WNDCLASSEXW ghost_window_class    = {};
    ghost_window_class.cbSize         = sizeof(ghost_window_class);
    ghost_window_class.lpfnWndProc    = &wGhostWindowProc;
    ghost_window_class.hInstance      = GetModuleHandleW(NULL);
    ghost_window_class.hIcon          = LoadIconA(NULL, IDI_APPLICATION);
    ghost_window_class.hCursor        = LoadCursorA(NULL, IDC_ARROW);
    ghost_window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    ghost_window_class.lpszClassName  = L"ghostWindowClass";
    RegisterClassExW(&ghost_window_class);

    // Our creation routine is fairly normal, except for the fact we don't specify
    // any styling; effectively making this window hidden. This is intentional, as
    // we only want this ghost window to capture messages for us.
    HWND ghost_window_handle = CreateWindowExW(0, ghost_window_class.lpszClassName,
            L"Ghost Window", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, ghost_window_class.hInstance, 0);
    ghost_window->window_handle = ghost_window_handle;
    
    // Signals back to the main thread after sync that it is valid, otherwise, it asserts.
    if (ghost_window_handle)
        ghost_window->ghost_thread_ready = true;
    else
        ExitProcess(1);

    // This signals back to the main thread that the ghost window thread's is done
    // doing its essential set up.
    SetEvent(ghost_window->ghost_thread_sync);

    // This is the 
    while(ghost_window->ghost_thread_runtime)
    {

        MSG current_message = {};
        GetMessageW(&current_message, 0, 0, 0);
        TranslateMessage(&current_message);

        // These messages are forwarded back to main to be captured.
        if ((current_message.message == WM_CHAR) ||
            (current_message.message == WM_QUIT) ||
            (current_message.message == WM_SIZE))
        {
            PostThreadMessageW(main_thread_id, current_message.message,
                    current_message.wParam, current_message.lParam);
        }
        else
        {
            DispatchMessageW(&current_message);
        }

    }


    // The ghost thread will probably never exit naturally, but this is here in
    // case it does.
    ExitProcess(0);
}

// --- Window API Definitions --------------------------------------------------
//
// Actual code that does actual work rather than code that fixes something that
// shouldn't be a problem in the first place. Yes, I know, I'm still ranting.
//



struct win32_window
{
    HWND    handle;
    i32     width;
    i32     height;
};

i32
window_width(j5_window window)
{

    win32_window *window_state = (win32_window*)window;
    return (window_state->width);

}

i32
window_height(j5_window window)
{

    win32_window *window_state = (win32_window*)window;
    return (window_state->height);
}

bool
window_update(j5_window window)
{

    // Change our handle to window state.
    win32_window* window_state = (win32_window*)window;

    // Essentially, we just poll the message loop.
    MSG current_message = {};
    while(PeekMessage(&current_message, 0, 0, 0, PM_REMOVE))
    {

        switch(current_message.message)
        {
            case WM_CLOSE:
            {
                window_close(window);
                return false;
            };  
            case WM_SIZE:
            {
                
                // Parse our the new dimensions.
                int width = (0xFFFF & current_message.lParam);
                int height = (current_message.lParam >> 16);
                
                // Update the current state.
                window_state->width = width;
                window_state->height = height;

            };
        }

    }

    return true;

}

void
window_close(j5_window window)
{

    // Emits back to the ghost window to close the window. This procedure is async and
    // may not happen immediately.
    SendMessageW(ghost_window->window_handle, UD_DESTROY_WINDOW,
            (WPARAM)&(((win32_window*)window)->handle), 0);

};

j5_window
window_create(const char *window_title, i32 width, i32 height)
{

    // NOTE(Chris): Since Microsoft designed their window API to be blocking during
    //              movement and resize events, we need to dedicate a thread specifically
    //              to capture this behavior so that it *doesn't* block the main thread.
    //              Microsoft believes that people wouldn't notice this in real-time apps,
    //              but I actually care about writing good software so the below code is
    //              in place to ensure that the problem doesn't occur.
    
    static bool ghost_initialized = false;
    if (ghost_initialized == false)
    {

        // Get the main thread's identifier.
        main_thread_id = GetCurrentThreadId();

        // Create our ghost window's state and initialize it.
        ghost_window = (win32_ghost_window*)malloc(sizeof(win32_ghost_window));
        ghost_window->ghost_thread_sync     = CreateEventA(NULL, FALSE, FALSE, NULL);
        ghost_window->ghost_thread_runtime  = true;
        ghost_window->ghost_thread_ready    = false;

        // Launch our thread.
        CreateThread(0, 0, ghost_window_main, NULL, 0, &ghost_window->ghost_thread_id);

        // We will now wait for the ghost thread to signal its init. We will essentially
        // wait until its done launching.
        WaitForSingleObject(ghost_window->ghost_thread_sync, INFINITE);
        CloseHandle(ghost_window->ghost_thread_sync);

        // If we reach here, the thread is ready, meaning its in a state that can
        // capture messages. However, it is possible it failed so we must check to
        // see if the gohst window is *actually* ready. We want to blow up in that case.
        assert(ghost_window->ghost_thread_ready == true);
        ghost_initialized = true;

    }

    // NOTE(Chris): Unlike normal window creation, we will use a seperate thread
    //              that generates windows for us. We basically wrap the function
    //              parameters of CreateWindowExW into a struct and send it off to
    //              the ghost thread to do the dirty work for us.

    WNDCLASSEXW display_window_class    = {};
    display_window_class.cbSize         = sizeof(display_window_class);
    display_window_class.lpfnWndProc    = &wDisplayWindowProc;
    display_window_class.hInstance      = GetModuleHandleW(NULL);
    display_window_class.hIcon          = LoadIconA(NULL, IDI_APPLICATION);
    display_window_class.hCursor        = LoadCursorA(NULL, IDC_ARROW);
    display_window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    display_window_class.lpszClassName  = L"displayWindowClass";
    RegisterClassExW(&display_window_class);

    // Calculate our window size from the requested client area, which are different
    // sizes since Windows doesn't like to make things intuitive and easy to understand.
    RECT client_rect = {};
    client_rect.right = width;
    client_rect.bottom = height;
    AdjustWindowRect(&client_rect, WS_OVERLAPPEDWINDOW, FALSE);

    // Definitely not the actual client area size.
    i32 window_width = client_rect.right - client_rect.left;
    i32 window_height = client_rect.bottom - client_rect.top;

    // Establish our creation context.
    win32_window_creation_context context   = {};
    context.ex_style            = 0;
    context.class_name          = display_window_class.lpszClassName;
    context.window_name         = L"J5 Window";
    context.style               = WS_OVERLAPPEDWINDOW;
    context.x                   = CW_USEDEFAULT;
    context.y                   = CW_USEDEFAULT;
    context.width               = window_width;
    context.height              = window_height;
    context.instance            = display_window_class.hInstance;

    HWND display_window_handle = (HWND)SendMessageW(ghost_window->window_handle,
            UD_CREATE_WINDOW, (WPARAM)&context, 0);

    // Since the CreateWindowExW expects a wide-char title but this creation function
    // is a standard 8-bit character string, we need to manually update the title
    // using the ansi version to match the requested title.
    SetWindowTextA(display_window_handle, window_title);

    // Now we just need to show the window.
    ShowWindow(display_window_handle, SW_SHOWNORMAL);

    // Finally, give the handle back to the user.
    win32_window *window_state = (win32_window*)malloc(sizeof(win32_window));
    window_state->width = width;
    window_state->height = height;
    return (j5_window)window_state;

};

#endif
