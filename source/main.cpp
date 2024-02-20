// -----------------------------------------------------------------------------
// CSCI 490-J5 Graphics Renderer
//      Christopher DeJong - Z1836870
// -----------------------------------------------------------------------------
// 
// Fully Win32, just as our lord and savior, Steve Balmer, would have wanted.
// Since this application runs using a terminal subsystem, it isn't intended to
// be ran as a standard GUI app. For terminal useage, supply a path to an rd-view
// file as the first parameter.
//
#include <windows.h>
#include <cstdio>

#include <core.h>
#include <window.h>
#include <rdview/parser.h>
#include <renderer/renderer.h>

// --- Window Procedure --------------------------------------------------------

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

// --- Process Messages --------------------------------------------------------
//
// This could be a GOTO, but I don't want to scare anyone.
//

static bool
process_messages(void)
{

    // Peek the window.
    MSG current_message = {};
    while (PeekMessageW(&current_message, 0, 0, 0, PM_REMOVE))
    {

        if (current_message.message == WM_QUIT)
        {
            return false;
        }

        TranslateMessage(&current_message);
        DispatchMessageW(&current_message);

    }

    return true;

}

// --- Main --------------------------------------------------------------------
//
// 1.   Checks for an rd-view script file provided by CLI.
// 2.   Launches window given the parameter of the rd-view file.
// 3.   Does some rendering magic.
// 4.   Busy spins until someone hits the close button.
//
// It just works, baby.
//

int
main(int argc, char ** argv)
{ 

    // Check for the provided file path.
    if (argc <= 1)
    {
        printf("Useage: j5 [rd-view source]\n");   
        return 1;
    }

    // Parse the rd view file.
    char* source_buffer = rdview_source_fetch(argv[1]);
    if (source_buffer == NULL)
    {
        printf("Unable to open rdview source file.\n");
        return 1;
    }

    rdview_configuration current_configuration = {};
    if (!rdview_source_parse(&current_configuration, source_buffer))
    {
        printf("Catastrophic parse failure for rdview source: %s.\n", argv[1]);
        return 1;
    }

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
    int request_width = current_configuration.state.device_width;
    int request_height = current_configuration.state.device_height;

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
        return 1;
    }

    // Change the window title.
    SetWindowTextA(window_handle, current_configuration.state.display_name);

    // Show the window.
    ShowWindow(window_handle, SW_SHOWNORMAL);

    // Setup device context to render.
    window_state *state = get_window_state();
    state->width        = request_width;
    state->height       = request_height;
    state->handle       = window_handle;
    state->device       = GetDC(window_handle);

#if 0
    // Test the set pixel function.
    for (i32 y = 0; y < request_height; ++y)
    {
        for (i32 x = 0; x < request_width; ++x)
        {
            set_pixel(x, y, (f32)x / request_width, (f32)y / request_height, 0.0f);
        }
    }
#endif

    // Okay, now we can process the operations.
    rdview_source_run(&current_configuration);

    // Pump the window.
    static bool runtime_flag = true;
    while (runtime_flag == true)
    {

        if (!process_messages())
            break;

    }

    return 0;
}

