#include <window.h>

window_state*
get_window_state()
{

    static window_state state;
    return &state;

}
