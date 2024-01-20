#include <iostream>
#include <window.h>

int
main(int argc, char ** argv)
{ 

    // Create a window to display our visual data to.
    j5_window window_handle = window_create("CSCI 490-J4/631", 800, 600);
    std::cout << "A window has been created with the dimensions: " << window_width(window_handle)
        << "px by " << window_height(window_handle) << "px." << std::endl;

    // This will automatically poll the window for updates.
    while (window_update(window_handle))
    {

    }

    return 0;
}

