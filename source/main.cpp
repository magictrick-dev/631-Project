#include <iostream>
//#include <window.h>
#include <window/window_handler.h>

int
main(int argc, char ** argv)
{ 
#if 0
    // Create a window to display our visual data to.
    j5_window window_handle = window_create("CSCI 490-J4/631", 800, 600);
    std::cout << "A window has been created with the dimensions: " << window_width(window_handle)
        << "px by " << window_height(window_handle) << "px." << std::endl;

    // This will automatically poll the window for updates.
    while (window_update(window_handle))
    {

    }
#endif

    // This must be called in order for windows to be created.
    if (!window_initialize_startup())
    {
        std::cout << "[ ERROR ] Unable to initialize window creation thread." << std::endl;
        return 1;
    }

    // Now, we create a window.
    j5win display_window = window_create("CSCI 490-J5 Renderer", 800, 600);
    if (display_window == NULL)
    {
        std::cout << "[ ERROR ] Unable to create the display window." << std::endl;
        return 1;
    }

    // This runtime loop will spin as long as windows are still active.
    while (window_process_updates())
    {

    }

    return 0;
}

