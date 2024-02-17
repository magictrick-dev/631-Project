#include <windows.h>
#include <window/window_handler.h>
#include <cstdlib>
#include <cstdio>

struct window_state
{
    HWND    handle;

    u32     width;
    u32     height;
};

struct ghost_thread
{
    BOOL    ghost_thread_runtime;
    BOOL    ghost_thread_ready;
    DWORD   ghost_thread_identifier;
    HANDLE  ghost_thread_sync;
    HWND    ghost_window;

    DWORD   windows_active;
};

struct window_creation_context
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

struct window_message
{
    UINT    message;
    WPARAM  w_param;
    LPARAM  l_param;
};

#define J5_WINDOW_CAST(handle) (window_state *)handle

#define UD_CREATE_WINDOW    (WM_USER + 0x0042)
#define UD_DESTROY_WINDOW   (WM_USER + 0x0043)
#define UD_UPDATE_WINDOW    (WM_USER + 0x0044)

static DWORD            main_thread_id;
static ghost_thread*    ghost_thread_state;

// --- Internal Helpers --------------------------------------------------------

static LRESULT CALLBACK
wDisplayWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

    LRESULT ret_result = 0;   

    switch (message)
    {
        
        // We need to attach our window user parameter to window long so we
        // can easily access it when messages are called.
        case WM_CREATE:
        {
            
            CREATESTRUCT *window_create = (CREATESTRUCT*)l_param;
            LPVOID window_user_parameter = window_create->lpCreateParams;
            SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)window_user_parameter);
            break;
        }

        // For close, we simply just smuggle the window handle with w_param.
        case WM_CLOSE:
        {
            PostThreadMessageW(main_thread_id, message, (WPARAM)window, l_param);
            break;
        };

        // For all other messages that we want to process, we just send it over.
        // They are sent as UD_UPDATE_WINDOW messages, packed with the original
        // message and the window it came from.
        case WM_SIZE:
        case WM_CHAR:
        case WM_DESTROY:
        {
            window_message *current_message = (window_message *)malloc(sizeof(window_message));
            current_message->message = message;
            current_message->w_param = w_param;
            current_message->l_param = l_param;
            PostThreadMessageW(main_thread_id, UD_UPDATE_WINDOW, (WPARAM)current_message, (LPARAM)window);
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
            window_creation_context *context = (window_creation_context*)w_param;
            HWND window_handle = CreateWindowExW(context->ex_style, context->class_name,
                    context->window_name, context->style, context->x, context->y, context->width,
                    context->height, context->parent_window, context->menu, context->instance,
                    context->user_parameter);

            // The result can be packed as a LRESULT, which is kinda neat.
            if (window_handle != NULL)
                ghost_thread_state->windows_active++;
            ret_result = (LRESULT)window_handle;
            break;

        };

        case UD_DESTROY_WINDOW:
        {

            // A request to destroy the window is made.
            if (DestroyWindow((HWND)w_param))
                ghost_thread_state->windows_active--;

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
window_ghost_main(LPVOID user_param)
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
    ghost_thread_state->ghost_window = ghost_window_handle;
    
    // Signals back to the main thread after sync that it is valid, otherwise, it asserts.
    if (ghost_window_handle)
        ghost_thread_state->ghost_thread_ready = true;
    else
        ExitProcess(1);

    // This signals back to the main thread that the ghost window thread's is done
    // doing its essential set up.
    SetEvent(ghost_thread_state->ghost_thread_sync);

    // This is the message pump that the windows all forward out.
    while(ghost_thread_state->ghost_thread_runtime)
    {

        MSG current_message = {};
        GetMessageW(&current_message, 0, 0, 0);
        TranslateMessage(&current_message);

        // These messages are forwarded back to main to be captured.
        if ((current_message.message == WM_CHAR) ||
            (current_message.message == WM_QUIT) ||
            (current_message.message == WM_SIZE))
        {
            window_message *pass_message = (window_message *)malloc(sizeof(window_message));
            pass_message->message = current_message.message;
            pass_message->w_param = current_message.wParam;
            pass_message->l_param = current_message.lParam;
            PostThreadMessageW(main_thread_id, UD_UPDATE_WINDOW, (WPARAM)pass_message,
                    (LPARAM)current_message.hwnd);
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

// --- Window Handler API ------------------------------------------------------
//
// Window initialize startup must be called before windows can be generated. An
// easy check can be done when the first call to create window is done or explicitly.
// A static reference to the state is made such that it is visible once complete.
//

bool
window_initialize_startup(void)
{

    // In the event this is called more than once, just return the ready status.
    if (ghost_thread_state != NULL)
        return ghost_thread_state->ghost_thread_ready;

    // We need to get the main thread identifier so our ghost thread
    // knows where to post messages.
    main_thread_id = GetCurrentThreadId();

    // Create the ghost thread state structure.
    ghost_thread_state = (ghost_thread*)malloc(sizeof(ghost_thread));
    ghost_thread_state->ghost_thread_sync       = CreateEventA(NULL, FALSE, FALSE, NULL);
    ghost_thread_state->ghost_thread_runtime    = true;
    ghost_thread_state->ghost_thread_ready      = false;
    ghost_thread_state->windows_active          = 0;        // Ghetto reference counting.

    // Create the thread.
    CreateThread(0, 0, window_ghost_main, NULL, 0, &ghost_thread_state->ghost_thread_identifier);

    // Join the thread. In order for a window to be generated, the ghost window
    // must first be initialized on ghost thread so it can capture window creation events.
    WaitForSingleObject(ghost_thread_state->ghost_thread_sync, INFINITE);
    CloseHandle(ghost_thread_state->ghost_thread_sync);

    // Ensure that the ghost thread is ready. The return value tells us that it is.
    return (ghost_thread_state->ghost_thread_ready);

}

bool
window_process_updates()
{

    if (ghost_thread_state->windows_active <= 0)
        return false;

    MSG current_message = {};
    while (PeekMessage(&current_message, 0, 0, 0, PM_REMOVE))
    {

        if (current_message.message == UD_UPDATE_WINDOW)
        {           

            HWND specified_window = (HWND)current_message.lParam;
            window_message *message = (window_message *)current_message.wParam;
            window_state *window = (window_state*)GetWindowLongPtr(specified_window, GWLP_USERDATA);

            if (message->message == WM_SIZE)
            {
                printf("Resizing.\n");
            }

            free(message);

        }

    }

    return true;

}

j5win
window_create(const char *window_name, u32 width, u32 height)
{

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

    // Create our window state.
    //window_state *window_state = (window_state*)(malloc(sizeof(window_state)));
    window_state *window = (window_state*)malloc(sizeof(window_state));

    // Establish our creation context.
    window_creation_context context = {};
    context.ex_style            = 0;
    context.class_name          = display_window_class.lpszClassName;
    context.window_name         = L"J5 Window";
    context.style               = WS_OVERLAPPEDWINDOW;
    context.x                   = CW_USEDEFAULT;
    context.y                   = CW_USEDEFAULT;
    context.width               = window_width;
    context.height              = window_height;
    context.instance            = display_window_class.hInstance;
    context.user_parameter      = (LPVOID)window;

    HWND display_window_handle = (HWND)SendMessageW(ghost_thread_state->ghost_window,
            UD_CREATE_WINDOW, (WPARAM)&context, 0);
    if (display_window_handle == NULL)
    {
        free(window);
        return NULL;
    }

    // Since the CreateWindowExW expects a wide-char title but this creation function
    // is a standard 8-bit character string, we need to manually update the title
    // using the ansi version to match the requested title.
    SetWindowTextA(display_window_handle, window_name);

    // Now we just need to show the window.
    ShowWindow(display_window_handle, SW_SHOWNORMAL);

    // Finally, give the handle back to the user.
    window->handle = display_window_handle;
    window->width = width;
    window->height = height;
    return (j5win)window;

}

void
window_close(j5win *window_handle)
{

    // Emits back to the ghost window to close the window. This procedure is async and
    // may not happen immediately.
    SendMessageW(ghost_thread_state->ghost_window, UD_DESTROY_WINDOW,
            (WPARAM)&(((window_state*)window_handle)->handle), 0);
    
}
