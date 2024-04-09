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
#include <iostream>
#include <cstdio>

#include <rdview/parser.h>

#include <renderer/device.h>
#include <renderer/renderer.h>

#include <core.h>

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
    char* source_buffer = rdview::get_source(argv[1]);
    if (source_buffer == NULL)
    {
        printf("Unable to open rdview source file.\n");
        return 1;
    }

    rdview render_description = { source_buffer };
    if (!render_description.init())
        return 1;
    if (!render_description.begin())
        return 1;

    // Create the depthbuffer using the render description.
    create_depthbuffer(render_description.width, render_description.height);

    if (render_description.device == RDVIEW_DEVICE_WINDOW)
    {

        window_device window = {    
            render_description.title,
            render_description.width,
            render_description.height
        };

        bool is_double_buffered = false;

        if (!window.init_window(is_double_buffered))
            return 1;

        render_description.active_device = &window;


        Sleep(500);

        // Okay, now we can process the operations.
        window.set_fill(render_description.canvas_color);
        render_description.render();

        if (is_double_buffered)
            window.swap_buffers();

        while (!window.should_close())
        {

        }

    }

    else if (render_description.device == RDVIEW_DEVICE_PNM)
    {
        pnm_device pnm = {
            render_description.title,
            render_description.width,
            render_description.height,
        };

        pnm.init_image();
        render_description.active_device = &pnm;
        pnm.set_fill(render_description.canvas_color);
        render_description.render();

        if (!pnm.save_image())
        {
            std::cout << "Unable to save file." << std::endl;
        }
        else
        {
            std::cout << "Saved successfully." << std::endl;
        }
    }

    return 0;
}

