#include <rdview/operations.h>
#include <rdview/statement.h>
#include <rdview/parser.h>
#include <renderer/renderer.h>

// --- Display -----------------------------------------------------------------
//
//
//

rddisplay::
rddisplay(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rddisplay::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for Display is incorrect.");
        return false;
    }
    
    // Get title.
    this->title = rdview_parser_keyword_dequote(stm[1]);

    // Get the device type.
    std::string device = rdview_parser_keyword_dequote(stm[2]);
    if (device == "Screen")
        this->device = RDVIEW_DEVICE_WINDOW;
    else if (device == "PNM")
        this->device = RDVIEW_DEVICE_PNM;
    else
    {
        stm.print_error("Display device is unrecognized.");
        return false;
    }

    if (this->device == RDVIEW_DEVICE_WINDOW)
    {
        std::string mode = rdview_parser_keyword_dequote(stm[3]);
        if (mode == "rgbsingle")
            this->mode = RDVIEW_DRAW_TYPE_SINGLE;
        else if (mode == "rgbdouble")
            this->mode = RDVIEW_DRAW_TYPE_DOUBLE;
        else if (mode == "rgbobject")
            this->mode = RDVIEW_DRAW_TYPE_STEP;
        else
        {
            stm.print_error("Display mode is unrecognized.");
            return false;
        }
    }
    else
    {
        // We can just do whatever here.
        this->mode = RDVIEW_DRAW_TYPE_OUTPUT;
    }

    return true;

}

void rddisplay::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->title  = this->title;
    rdv->device = this->device;
    rdv->mode   = this->mode;

    return;

}

// --- Format ------------------------------------------------------------------
//
//
//

rdformat::
rdformat(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdformat::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 3)
    {
        stm.print_error("The number of arguments for Format is incorrect.");
        return false;
    }
    
    this->width = std::stoi(stm[1]);
    this->height = std::stoi(stm[2]);

    return true;

}

void rdformat::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->width = this->width;
    rdv->height = this->height;

    return;

}

// --- Background --------------------------------------------------------------
//
//
//

rdbackground::
rdbackground(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdbackground::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for Background is incorrect.");
        return false;
    }
 
    f32 r = std::stof(stm[1]);
    f32 g = std::stof(stm[2]);
    f32 b = std::stof(stm[3]);
    this->color = { r, g, b };

    return true;

}

void rdbackground::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->canvas_color = this->color;

    return;

}

// --- Color -------------------------------------------------------------------
//
//
//

rdcolor::
rdcolor(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcolor::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for Color is incorrect.");
        return false;
    }
 
    f32 r = std::stof(stm[1]);
    f32 g = std::stof(stm[2]);
    f32 b = std::stof(stm[3]);
    this->color = { r, g, b };

    return true;

}

void rdcolor::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->draw_color = this->color;

    return;

}

// --- Line --------------------------------------------------------------------

rdline::
rdline(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdline::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 7)
    {
        stm.print_error("The number of arguments for Line is incorrect.");
        return false;
    }
 
    this->x1 = std::stoi(stm[1]);
    this->y1 = std::stoi(stm[2]);
    this->z1 = std::stoi(stm[3]);
    this->x2 = std::stoi(stm[4]);
    this->y2 = std::stoi(stm[5]);
    this->z2 = std::stoi(stm[6]);

    return true;

}

void rdline::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    set_line(rdv->active_device, this->x1, this->x2, this->y1, this->y2, this->z1, this->z2, rdv->draw_color);

    return;

}

// --- Point -------------------------------------------------------------------

rdpoint::
rdpoint(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdpoint::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for Point is incorrect.");
        return false;
    }
 
    this->x = std::stoi(stm[1]);
    this->y = std::stoi(stm[2]);
    this->z = std::stoi(stm[3]);

    return true;

}

void rdpoint::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    set_point(rdv->active_device, this->x, this->y, this->z, rdv->draw_color);

    return;

}

// --- Circle ------------------------------------------------------------------

rdcircle::
rdcircle(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcircle::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 5)
    {
        stm.print_error("The number of arguments for Circle is incorrect.");
        return false;
    }
 
    this->x = std::stoi(stm[1]);
    this->y = std::stoi(stm[2]);
    this->z = std::stoi(stm[3]);
    this->r = std::stoi(stm[4]);

    return true;

}

void rdcircle::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    set_circle(rdv->active_device, this->x, this->y, this->z, this->r, rdv->draw_color);

    return;

}

// --- Flood -------------------------------------------------------------------

rdflood::
rdflood(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdflood::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for Fill is incorrect.");
        return false;
    }
 
    this->x = std::stoi(stm[1]);
    this->y = std::stoi(stm[2]);
    this->z = std::stoi(stm[3]);

    return true;

}

void rdflood::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    set_flood(rdv->active_device, this->x, this->y, this->z, rdv->draw_color);

    return;

}

// --- World -------------------------------------------------------------------

rdworldbegin::
rdworldbegin(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdworldbegin::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 1)
    {
        stm.print_error("The number of arguments for World is incorrect.");
        return false;
    }
 
    return true;

}

void rdworldbegin::
execute()
{

    return;

}

// --- World End ---------------------------------------------------------------

rdworldend::
rdworldend(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdworldend::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 1)
    {
        stm.print_error("The number of arguments for World is incorrect.");
        return false;
    }
 
    return true;

}

void rdworldend::
execute()
{

    return;

}
