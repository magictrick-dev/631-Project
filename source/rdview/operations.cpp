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
 
    this->x1 = std::stof(stm[1]);
    this->y1 = std::stof(stm[2]);
    this->z1 = std::stof(stm[3]);

    this->x2 = std::stof(stm[4]);
    this->y2 = std::stof(stm[5]);
    this->z2 = std::stof(stm[6]);

    return true;

}

void rdline::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    //set_line(rdv->active_device, this->x1, this->x2, this->y1, this->y2, this->z1, this->z2, rdv->draw_color);
    rdv->rd_line_pipeline({(f32)this->x1, (f32)this->y1, (f32)this->z1}, true);
    rdv->rd_line_pipeline({(f32)this->x2, (f32)this->y2, (f32)this->z2}, false);

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
 
    this->x = std::stof(stm[1]);
    this->y = std::stof(stm[2]);
    this->z = std::stof(stm[3]);

    return true;

}

void rdpoint::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    //set_point(rdv->active_device, this->x, this->y, this->z, rdv->draw_color);
    rdv->rd_point_pipeline({(f32)this->x, (f32)this->y, (f32)this->z}, false);

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

    rdview *rdv = (rdview*)this->rdview_parent;

    // Set up the pipeline matrices.
    v4 ce;
    ce.xyz = rdv->camera_eye;
    ce.w = 1.0f;

    v4 ca;
    ca.xyz = rdv->camera_at;
    ca.w = 1.0f;

    v4 cu;
    cu.xyz = rdv->camera_up;
    cu.w = 1.0f;

    rdv->world_to_camera = m4::create_world_to_camera(ce, ca, cu);
    rdv->camera_to_clip = m4::create_camera_to_clip(rdv->fov, rdv->nearp,
            rdv->farp, ((f32)rdv->width / (f32)rdv->height));
    rdv->clip_to_device = m4::create_clip_to_device(rdv->width, rdv->height);

    std::cout << "World to Camera:\n" << rdv->world_to_camera << std::endl << std::endl;
    std::cout << "Camera to Clip:\n" << rdv->camera_to_clip << std::endl << std::endl;
    std::cout << "Clip to Device:\n" << rdv->clip_to_device << std::endl << std::endl;

    // Reset the transform stack.
    rdv->transform_stack.clear();

    // Set the current transform.
    rdv->current_transform = m4::create_identity();

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

// --- Camera Eye --------------------------------------------------------------

rdcameraeye::
rdcameraeye(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcameraeye::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for camera eye is incorrect.");
        return false;
    }
 
    this->eye = { std::stof(stm[1]), std::stof(stm[2]), std::stof(stm[3]) };

    return true;

}

void rdcameraeye::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->camera_eye = this->eye;

    return;

}

// --- Camera Eye --------------------------------------------------------------

rdcameraat::
rdcameraat(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcameraat::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for camera at is incorrect.");
        return false;
    }
 
    this->at = { std::stof(stm[1]), std::stof(stm[2]), std::stof(stm[3]) };

    return true;

}

void rdcameraat::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->camera_at = this->at;

    return;

}

// --- Camera Up ---------------------------------------------------------------

rdcameraup::
rdcameraup(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcameraup::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for camera up is incorrect.");
        return false;
    }
 
    this->up = { std::stof(stm[1]), std::stof(stm[2]), std::stof(stm[3]) };

    return true;

}

void rdcameraup::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->camera_up = this->up;

    return;

}

// --- Translation -------------------------------------------------------------

rdtranslation::
rdtranslation(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdtranslation::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for translation is incorrect.");
        return false;
    }
 
    this->x = std::stof(stm[1]);
    this->y = std::stof(stm[2]);
    this->z = std::stof(stm[3]);

    return true;

}

void rdtranslation::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    m4 translation = m4::create_transform({this->x, this->y, this->z});
    //rdv->transform_stack.push_back(translation);

    rdv->current_transform = rdv->current_transform * translation;

    return;

}

// --- Scale -------------------------------------------------------------------

rdscale::
rdscale(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdscale::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for scale is incorrect.");
        return false;
    }
 
    this->x = std::stof(stm[1]);
    this->y = std::stof(stm[2]);
    this->z = std::stof(stm[3]);

    return true;

}

void rdscale::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    m4 scale = m4::create_scale({this->x, this->y, this->z});
    //rdv->transform_stack.push_back(scale);

    rdv->current_transform = rdv->current_transform * scale;

    return;

}

// --- Camera FOV --------------------------------------------------------------

rdcamerafov::
rdcamerafov(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcamerafov::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 2)
    {
        stm.print_error("The number of arguments for FOV is incorrect.");
        return false;
    }
 
    this->fov = std::stof(stm[1]);

    return true;

}

void rdcamerafov::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->fov = this->fov;

    return;

}


// --- Cube --------------------------------------------------------------------

rdcube::
rdcube(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcube::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 1)
    {
        stm.print_error("The number of arguments for cube is incorrect.");
        return false;
    }
 
    return true;

}

void rdcube::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;

    // Front
    rdv->rd_line_pipeline({-1.0f, -1.0f, 1.0f}, true);
    rdv->rd_line_pipeline({1.0f, -1.0f, 1.0f}, false);
    rdv->rd_line_pipeline({1.0f, 1.0f, 1.0f}, false);
    rdv->rd_line_pipeline({-1.0f, 1.0f, 1.0f}, false);
    rdv->rd_line_pipeline({-1.0f, -1.0f, 1.0f}, false);

    // Left
    rdv->rd_line_pipeline({-1.0f, 1.0f, 1.0f}, false);
    rdv->rd_line_pipeline({-1.0f, 1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({-1.0f, -1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({-1.0f, -1.0f, 1.0f}, false);

    // Bottom
    rdv->rd_line_pipeline({-1.0f, -1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({1.0f, -1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({1.0f, -1.0f, 1.0f}, false);

    // Right
    rdv->rd_line_pipeline({1.0f, -1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({1.0f, 1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({1.0f, 1.0f, 1.0f}, false);

    // Top
    rdv->rd_line_pipeline({1.0f, 1.0f, -1.0f}, false);
    rdv->rd_line_pipeline({-1.0f, 1.0f, -1.0f}, false);

    return;

}

// --- Xform Push --------------------------------------------------------------

rdxformpush::
rdxformpush(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdxformpush::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 1)
    {
        stm.print_error("The number of arguments for xform push is incorrect.");
        return false;
    }
 
    return true;

}

void rdxformpush::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    rdv->transform_stack.push_back(rdv->current_transform);

    return;

}

// --- Xform Pop ---------------------------------------------------------------

rdxformpop::
rdxformpop(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdxformpop::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 1)
    {
        stm.print_error("The number of arguments for xform push is incorrect.");
        return false;
    }
 
    return true;

}

void rdxformpop::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;

    assert(rdv->transform_stack.size() >= 1);

    m4 last_transform = rdv->transform_stack.back();
    rdv->current_transform = last_transform;
    rdv->transform_stack.pop_back();

    return;

}

// --- Sphere ------------------------------------------------------------------

rdsphere::
rdsphere(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdsphere::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 5)
    {
        stm.print_error("The number of arguments for sphere is incorrect.");
        return false;
    }
 
    this->zmin      = std::stof(stm[1]);
    this->zmax      = std::stof(stm[2]);
    this->r         = std::stof(stm[3]);
    this->theta     = std::stof(stm[4]);

    return true;

}

void rdsphere::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;

    // Draw x-axis.
    {
        size_t step = 0;
        f32 theta = ((f32)step / 30) * this->theta;
        f32 x = this->r * cosf(DEGREES_TO_RADIANS(theta));
        f32 y = this->r * sinf(DEGREES_TO_RADIANS(theta));
        rdv->rd_line_pipeline({x, y, 0.0f}, true);
        for (step = 1; step < 30; ++step)
        {
            theta = ((f32)step / 30) * this->theta;
            x = this->r * cosf(DEGREES_TO_RADIANS(theta));
            y = this->r * sinf(DEGREES_TO_RADIANS(theta));
            rdv->rd_line_pipeline({x, y, 0.0f}, false);
        }
        theta = ((f32)step / 30) * this->theta;
        x = this->r * cosf(DEGREES_TO_RADIANS(theta));
        y = this->r * sinf(DEGREES_TO_RADIANS(theta));
        rdv->rd_line_pipeline({x, y, 0.0f}, false);
    }

    {
        size_t step = 0;
        f32 theta = ((f32)step / 30) * this->theta;
        f32 x = this->r * cosf(DEGREES_TO_RADIANS(theta));
        f32 y = this->r * sinf(DEGREES_TO_RADIANS(theta));
        rdv->rd_line_pipeline({0.0f, x, y}, true);
        for (step = 1; step < 30; ++step)
        {
            theta = ((f32)step / 30) * this->theta;
            x = this->r * cosf(DEGREES_TO_RADIANS(theta));
            y = this->r * sinf(DEGREES_TO_RADIANS(theta));
            rdv->rd_line_pipeline({0.0f, x, y}, false);
        }
        theta = ((f32)step / 30) * this->theta;
        x = this->r * cosf(DEGREES_TO_RADIANS(theta));
        y = this->r * sinf(DEGREES_TO_RADIANS(theta));
        rdv->rd_line_pipeline({0.0f, x, y}, false);
    }

    {
        size_t step = 0;
        f32 theta = ((f32)step / 30) * this->theta;
        f32 x = this->r * cosf(DEGREES_TO_RADIANS(theta));
        f32 y = this->r * sinf(DEGREES_TO_RADIANS(theta));
        rdv->rd_line_pipeline({y, 0.0f, x}, true);
        for (step = 1; step < 30; ++step)
        {
            theta = ((f32)step / 30) * this->theta;
            x = this->r * cosf(DEGREES_TO_RADIANS(theta));
            y = this->r * sinf(DEGREES_TO_RADIANS(theta));
            rdv->rd_line_pipeline({y, 0.0f, x}, false);
        }
        theta = ((f32)step / 30) * this->theta;
        x = this->r * cosf(DEGREES_TO_RADIANS(theta));
        y = this->r * sinf(DEGREES_TO_RADIANS(theta));
        rdv->rd_line_pipeline({y, 0.0f, x}, false);
    }

    return;

}

// --- Rotate ------------------------------------------------------------------

rdrotate::
rdrotate(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdrotate::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 3)
    {
        stm.print_error("The number of arguments for rotate is incorrect.");
        return false;
    }
 
    std::string axis = rdview_parser_keyword_dequote(stm[1]);
    if (axis == "X") this->axis = 0;
    else if (axis == "Y") this->axis = 1;
    else if (axis == "Z") this->axis = 2;
    else
    {
        stm.print_error("Unrecognized rotation axis for rotate.");
        return false;
        
    }

    this->theta = std::stof(stm[2]);

    return true;

}

void rdrotate::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;

    if (this->axis == 0)
    {
        m4 rx = m4::create_rotation_x(this->theta);
        rdv->current_transform = rdv->current_transform * rx;
    }

    else if (this->axis == 1)
    {
        m4 rx = m4::create_rotation_y(this->theta);
        rdv->current_transform = rdv->current_transform * rx;
    }

    else if (this->axis == 2)
    {
        m4 rx = m4::create_rotation_z(this->theta);
        rdv->current_transform = rdv->current_transform * rx;
    }

    else
    {
        assert(!"Unreachable, this is an error.");
    }

    return;

}

// --- RDObjectBegin -----------------------------------------------------------

rdobjectbegin::
rdobjectbegin(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdobjectbegin::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);
    rdview *rdv = (rdview*)this->rdview_parent;

    // Check for correct number of parameters.
    if (stm.count() != 2)
    {
        stm.print_error("The number of arguments for object begin is incorrect.");
        return false;
    }
 
    std::string name = rdview_parser_keyword_dequote(stm[1]);
    this->object.name = name;
    this->parse_index = stm.get_line_number();

    bool found = false;
    for (size_t i = 0; i < rdv->objects.size(); ++i)
    {
        if (rdv->objects[i].name == this->object.name)
        {
            found = true;
            break;
        }
    }

    if (found == true)
    {
        stm.print_error("The object of this name already exists.");
        return false;
    }

    rdv->parse_index++;
    while (rdv->parse_index < rdv->statements.size())
    {
        
        // Get the current statement.
        rdstatement *current_statement = &rdv->statements[rdv->parse_index];

        // If its the object end, we're done.
        if (current_statement->get_identifier() == "ObjectEnd")
        {
            break;
        }

        // Otherwise, move that parse index for the parser.
        else
        {
            rdv->parse_index++;
        }
        
        // Finally, create the operation adn push it back.
        rdoperation *objop = rdv->create_operation(current_statement);
        if (objop != NULL)
            this->object.operations.push_back(objop);
    }
    rdv->parse_index--;

    rdv->objects.push_back(this->object);    
#if 0
    // Go through all the operations.
    bool valid = true;
    for (size_t idx = this->parse_index; idx < rdv->statements.size(); ++idx)
    {

        // Get the current statement.
        rdstatement *current_statement = &rdv->statements[idx];

        // No nested object begins!
        if (current_statement->get_identifier() != "ObjectBegin")
        {
            current_statement->print_error("Nested object definitions are not allowed!");
            valid = false;
            break;
        }

        // If its the object end, we're done.
        if (current_statement->get_identifier() == "ObjectEnd")
            break;

        // Otherwise, move that parse index for the parser.
        else
        {
            rdv->parse_index++;
        }

        // Finally, create the operation adn push it back.
        rdoperation *objop = rdv->create_operation(current_statement);
        if (objop != NULL)
            this->object.operations.push_back(objop);

    }

    // If we encounter cases that cant happen, we simply return false.
    if (valid == false)
        return false;


    // Push back the object into the list of objects. We do this outside of the
    // execute because this is a top-to-bottom accessor.
    rdv->objects.push_back(this->object);    

#endif
    return true;

}

void rdobjectbegin::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;   

    return;

}

// --- Object End --------------------------------------------------------------

rdobjectend::
rdobjectend(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdobjectend::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 1)
    {
        stm.print_error("The number of arguments for object end is incorrect.");
        return false;
    }
 
    return true;

}

void rdobjectend::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;
    return;

}

// --- Object Instance ---------------------------------------------------------

rdobjectinstance::
rdobjectinstance(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdobjectinstance::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);
    rdview *rdv = (rdview*)this->rdview_parent;

    // Check for correct number of parameters.
    if (stm.count() != 2)
    {
        stm.print_error("The number of arguments for object instance is incorrect.");
        return false;
    }
 
    std::string name = rdview_parser_keyword_dequote(stm[1]);
    this->object_name = name;

    for (rdobject& obj: rdv->objects)
    {
        if (obj.name == this->object_name)
        {
            this->object = &obj;
            break;
        }
    }

    if (this->object == NULL)
    {
        stm.print_error("Invoking an instance of an object that has not yet been defined.");
        return false;
    }

    return true;

}

void rdobjectinstance::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;
    this->object->run();

    return;

};

// --- Poly Set ----------------------------------------------------------------

rdpolyset::
rdpolyset(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdpolyset::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);
    rdview *rdv = (rdview*)this->rdview_parent;

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for polyset is incorrect.");
        return false;
    }
 
    std::string type = rdview_parser_keyword_dequote(stm[1]);
    this->type = type;
    assert(type == "P");

    this->verts = std::stoi(stm[2]);
    this->fc = std::stoi(stm[3]);

    // We need to look ahead now.
    size_t vidx = 0;
    while (rdv->parse_index < rdv->statements.size())
    {
        
        // Get the current statement.
        rdstatement &current_statement = rdv->statements[rdv->parse_index];

        if (vidx >= this->verts)
            break;

        // Now check the verts.
        if (this->type == "P")
        {
            f32 x, y, z;
            x = std::stof(current_statement[0]);
            y = std::stof(current_statement[1]);
            z = std::stof(current_statement[2]);
            this->points.push_back({x, y, z});
        }
        else
        {
            assert(!"Unimplemented");
        }

        vidx++;
        rdv->parse_index++;

    }
    
    size_t fidx = 0;
    while (rdv->parse_index < rdv->statements.size())
    {

        // Get the current statement.
        rdstatement &current_statement = rdv->statements[rdv->parse_index];

        if (fidx >= this->fc)
            break;

        std::vector<int>& face_indicies = this->faces.emplace_back();
        size_t i = 0;
        while (i < current_statement.count())
        {
            i32 f = std::stoi(current_statement[i]);
            if (f == -1)
                break;
            face_indicies.push_back(f);
            i++;
        }

        fidx++;
        rdv->parse_index++;

    }
    rdv->parse_index--;
    return true;

}

void rdpolyset::
execute()
{

    rdview *rdv = (rdview*)this->rdview_parent;

    for (auto& current_face : this->faces)
    {

        rdv->rd_line_pipeline(this->points[current_face[0]], true);
        for (size_t i = 1; i < current_face.size(); ++i)
        {
            rdv->rd_line_pipeline(this->points[current_face[i]], false);
        }
        rdv->rd_line_pipeline(this->points[current_face[0]], false);


    }

    return;

};

// --- Cone --------------------------------------------------------------------

rdcone::
rdcone(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcone::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 4)
    {
        stm.print_error("The number of arguments for cone is incorrect.");
        return false;
    }
 
    this->h = std::stof(stm[1]);
    this->r = std::stof(stm[2]);
    this->theta = std::stof(stm[3]);

    return true;

}

void rdcone::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;

    for (size_t i = 0; i < 20; ++i)
    {

        f32 ctheta = (i / 20.0f) * this->theta;
        f32 cx = cosf(DEGREES_TO_RADIANS(ctheta));
        f32 cy = sinf(DEGREES_TO_RADIANS(ctheta));

        f32 ntheta = ((i + 1) / 20.0f) * this->theta;
        f32 nx = cosf(DEGREES_TO_RADIANS(ntheta));
        f32 ny = sinf(DEGREES_TO_RADIANS(ntheta));

        rdv->rd_line_pipeline({cx, cy, 0.0f}, true);
        rdv->rd_line_pipeline({0.0f, 0.0f, this->h}, false);
        rdv->rd_line_pipeline({nx, ny, 0.0f}, false);
        rdv->rd_line_pipeline({cx, cy, 0.0f}, false);

    }

#if 0
    bool move = true;

    f32 n1 = (1.0f / 20.0f) * this->theta;
    f32 x1 = this->r * cosf(DEGREES_TO_RADIANS(n1));
    f32 y1 = this->r * sinf(DEGREES_TO_RADIANS(n1));

    rdv->rd_line_pipeline({this->r, 0.0f, 0.0f}, true);
    rdv->rd_line_pipeline({0.0f, 0.0f, this->h}, false);
    rdv->rd_line_pipeline({x1, y1, 0.0f}, false);
    rdv->rd_line_pipeline({this->r, 0.0f, 0.0f}, true);

    for (size_t i = 1; i < 20; ++i)
    {

        f32 sub_theta = theta * ((f32)i / (f32)20);

        f32 x = this->r * cosf(DEGREES_TO_RADIANS(sub_theta));
        f32 y = this->r * sinf(DEGREES_TO_RADIANS(sub_theta));

        rdv->rd_line_pipeline({x, y, 0.0f}, true);
        rdv->rd_line_pipeline({0.0f, 0.0f, this->h}, false);
        rdv->rd_line_pipeline({x, y, 0.0f}, true);

        f32 ntheta = theta * ((f32)i+1 / 20.0f);
        f32 xn = this->r * cosf(DEGREES_TO_RADIANS(ntheta));
        f32 yn = this->r * sinf(DEGREES_TO_RADIANS(ntheta));
        rdv->rd_line_pipeline({xn, yn, 0.0f}, false);
        rdv->rd_line_pipeline({x, y, 0.0f}, true);

    }

    rdv->rd_line_pipeline({this->r, 0.0f, 0.0f}, false);
#endif

    return;

}

// --- Cylinder ----------------------------------------------------------------

rdcylinder::
rdcylinder(void *parent)
{

    // Set the parent.
    this->rdview_parent = parent;

}

bool rdcylinder::
parse(void *statement)
{

    rdstatement &stm = *((rdstatement*)statement);

    // Check for correct number of parameters.
    if (stm.count() != 5)
    {
        stm.print_error("The number of arguments for cylinder is incorrect.");
        return false;
    }
 
    this->r = std::stof(stm[1]);
    this->zmin = std::stof(stm[2]);
    this->zmax = std::stof(stm[3]);
    this->theta = std::stof(stm[4]);

    return true;

}

void rdcylinder::
execute()
{

    
    rdview *rdv = (rdview*)this->rdview_parent;

    for (size_t i = 0; i < 20; ++i)
    {

        f32 ctheta = (i / 20.0f) * this->theta;
        f32 cx = cosf(DEGREES_TO_RADIANS(ctheta));
        f32 cy = sinf(DEGREES_TO_RADIANS(ctheta));

        f32 ntheta = ((i + 1) / 20.0f) * this->theta;
        f32 nx = cosf(DEGREES_TO_RADIANS(ntheta));
        f32 ny = sinf(DEGREES_TO_RADIANS(ntheta));

        rdv->rd_line_pipeline({cx, cy, 0.0f}, true); 
        rdv->rd_line_pipeline({nx, ny, 0.0f}, false); 
        rdv->rd_line_pipeline({nx, ny, this->zmax}, false); 
        rdv->rd_line_pipeline({cx, cy, this->zmax}, false); 
        rdv->rd_line_pipeline({cx, cy, 0.0f}, false); 

    }

#if 0
    bool move = true;

    f32 n1 = (1.0f / 20.0f) * this->theta;
    f32 x1 = this->r * cosf(DEGREES_TO_RADIANS(n1));
    f32 y1 = this->r * sinf(DEGREES_TO_RADIANS(n1));

    rdv->rd_line_pipeline({this->r, 0.0f, 0.0f}, true);
    rdv->rd_line_pipeline({0.0f, 0.0f, this->h}, false);
    rdv->rd_line_pipeline({x1, y1, 0.0f}, false);
    rdv->rd_line_pipeline({this->r, 0.0f, 0.0f}, true);

    for (size_t i = 1; i < 20; ++i)
    {

        f32 sub_theta = theta * ((f32)i / (f32)20);

        f32 x = this->r * cosf(DEGREES_TO_RADIANS(sub_theta));
        f32 y = this->r * sinf(DEGREES_TO_RADIANS(sub_theta));

        rdv->rd_line_pipeline({x, y, 0.0f}, true);
        rdv->rd_line_pipeline({0.0f, 0.0f, this->h}, false);
        rdv->rd_line_pipeline({x, y, 0.0f}, true);

        f32 ntheta = theta * ((f32)i+1 / 20.0f);
        f32 xn = this->r * cosf(DEGREES_TO_RADIANS(ntheta));
        f32 yn = this->r * sinf(DEGREES_TO_RADIANS(ntheta));
        rdv->rd_line_pipeline({xn, yn, 0.0f}, false);
        rdv->rd_line_pipeline({x, y, 0.0f}, true);

    }

    rdv->rd_line_pipeline({this->r, 0.0f, 0.0f}, false);
#endif

    return;

}
