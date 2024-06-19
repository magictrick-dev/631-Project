#include <rdview/parser.h>
#include <renderer/renderer.h>
#include <core.h>
#include <windows.h>

// I wouldn't normally use C++ STL stuff, but I ain't got time to C-style a parser
// like the nerd that I am.
#include <string>
#include <iostream>
#include <sstream>
#include <vector>



// --- RDView Pipelines --------------------------------------------------------

void rdview::
rd_point_pipeline(v3 point, bool move)
{

    v4 h_point;
    h_point.xyz = point;
    h_point.w   = 1.0f;

    v4 result = this->clip_to_device * this->camera_to_clip * this->world_to_camera * this->current_transform * h_point;
    if (result.w >= 0)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
        result.w /= result.w;

        if (set_depthbuffer(result.x, result.y, result.z))
            set_point(this->active_device, result.x, result.y, result.z, this->draw_color);
        this->rd_point_state = result;
    }


}

void rdview::
rd_line_pipeline(v3 point, bool move)
{


    v4 h_point;
    h_point.xyz = point;
    h_point.w   = 1.0f;

    v4 result = this->camera_to_clip * this->world_to_camera * this->current_transform * h_point;

    if (move == true)
    {
        this->rd_line_state = result;
        return;
    }
    else
    {
        // this->clip_to_device   

        v4 a = this->rd_line_state;
        v4 b = result;


        if (line_clip(&a, &b))
        {
            v4 final_a = this->clip_to_device * a;
            v4 final_b = this->clip_to_device * b;

            final_a.x = final_a.x / final_a.w;
            final_a.y = final_a.y / final_a.w;
            final_a.z = final_a.z / final_a.w;
            final_a.w = final_a.w / final_a.w;

            final_b.x = final_b.x / final_b.w;
            final_b.y = final_b.y / final_b.w;
            final_b.z = final_b.z / final_b.w;
            final_b.w = final_b.w / final_b.w;

            set_line_dda(this->active_device, final_a, final_b, this->draw_color);

        }

        this->rd_line_state = result;

    }

    //set_line(this->active_device, 100, 500, 100, 300, 0, 0, this->draw_color);
    //this->rd_line_state = result;

}

void rdview::
rd_poly_pipeline(attr_point p, bool end_flag)
{

    // Maintain the list of vertices.
    static std::vector<attr_point> vertex_list;
    static std::vector<attr_point> vtx_normals;

    // We can actually just collect our attr_points as is as a seperate set.
    vtx_normals.push_back(p);

    // Compute the point and update the vertex position.
    v4 result = this->current_transform * p.position;
    p.world = result.xyz;
    p.constant = 1.0f;

    v4 normalh = {};
    normalh.xyz = p.normals;
    normalh.w = 1.0f;
    normalh = homogenize(this->lighting.light_transform * normalh);
    p.normals = normalh.xyz;

    // Finally, go to clip space.
    p.position = this->camera_to_clip * this->world_to_camera * result;

    // Place the vertex in the list.
    vertex_list.push_back(p);

    // If the end-flag is triggered, we need to render the polygon.
    if (end_flag == false)
        return;
    else
    {
 
        this->lighting.poly_normal = this->lighting.light_transform * this->lighting.poly_normal;

        // Clip the polygon and if there is anything there, draw it.
        std::vector<attr_point> clip_list;
        if (poly_clip(vertex_list, clip_list))
        {
 
            // Convert to device coordinates and homogenize.
            for (size_t i = 0; i < clip_list.size(); ++i)
            {
                clip_list[i].position = this->clip_to_device * clip_list[i].position;

                for (size_t x = 0; x < 16; ++x)
                {
                    clip_list[i].coord[x] = clip_list[i].coord[x] / clip_list[i].position.w;
                }
            }

            scan_convert(this->active_device, clip_list, this->draw_color, this->lighting);

        }

        // Empty the vertex list once its rendered.
        vertex_list.clear();
        vtx_normals.clear();
    }

};

// --- RDView Parser Helpers ---------------------------------------------------

void
rdview_parser_strip_whitespace(std::string &line)
{

    // Get the first non-whitespace character.
    i32 index = -1;
    for (size_t idx = 0; idx < line.length(); ++idx)
    {
        if (isspace(line[idx]))
            index = idx;
        else
            break;
    }

    if (index == -1)
        return;

    line.erase(0, index - 1);

}

void
rdview_parser_rejoin_strings(std::vector<std::string>& lk_line)
{

    size_t idx = 0;
    while (idx < lk_line.size())
    {

        std::string& current = lk_line[idx];
        if (current[0] == '"' && current[current.length()-1] != '"')
        {
            while (idx < lk_line.size())
            {
                current += " ";
                current += lk_line[++idx];
                lk_line.erase(lk_line.begin()+idx);
                idx--;
                if (current[current.length()-1] == '"')
                    break;
            }
        }

        idx++;

    }

}

std::string
rdview_parser_keyword_dequote(std::string keyword)
{
    return keyword.substr(1, keyword.length()-2);
}


// --- The Parser --------------------------------------------------------------
//
// This is going to be scuffed, guaranteed.
//

char * rdview::
get_source(const char *file_path)
{
    
    // Open the file.
    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    // Check if it is open.
    if (file_handle == INVALID_HANDLE_VALUE)
        return NULL;

    // Now, get the file size.
    LARGE_INTEGER fsize = {};
    GetFileSizeEx(file_handle, &fsize);
    size_t file_size = (size_t)fsize.QuadPart;

    // Create room to read the file.
    char * file_buffer = (char*)malloc(file_size + 1);
    file_buffer[file_size] = '\0';

    // Read the file.
    ReadFile(file_handle, file_buffer, file_size, NULL, NULL);

    return file_buffer;

}

void rdview::
free_source(char * source_buffer)
{

    if (source_buffer != NULL)
    {
        free(source_buffer);
        source_buffer = NULL;
    }

    return;

}

rdview::
rdview(const char *source)
{

    std::stringstream source_stream { source };
    std::string line;

    // Pull out each line, then construct a list of statements (non-empty, non-comments).
    i32 current_line = 1;
    while (std::getline(source_stream, line))
    {
        this->source_lines.push_back(line);
        
        // Before generating, trim whitespace.
        rdview_parser_strip_whitespace(line);
        if (!line.empty() && line[0] != '#')
            this->statements.push_back({current_line, line});

        current_line++;
    }
    
    return;
}

bool rdview::
begin()
{

    if (this->operations[0]->optype != RDVIEW_OPTYPE_DISPLAY)
        return false;

    this->operations[0]->execute();

    size_t idx = 1;
    for (; idx < this->operations.size(); idx++)
    {

        if (this->operations[idx]->optype == RDVIEW_OPTYPE_FRAMEBEGIN)
        {
            this->start = idx;
            return true;
        }

        this->operations[idx]->execute();

    }

    return true;

}

void rdview::
render()
{

    for (size_t idx = start; idx < this->operations.size(); ++idx)
    {
        this->operations[idx]->execute();
    }

}

rdoperation* rdview::
create_operation(rdstatement *current_statement)
{

    std::string identifier = current_statement->get_identifier();
    if (identifier == "Display")
    {
        rddisplay *display = new rddisplay(this);
        if (!display->parse(current_statement))
            return NULL;
        display->optype = RDVIEW_OPTYPE_DISPLAY;
        return display;
    }

    else if (identifier == "Format")
    {

        rdformat *format = new rdformat(this);
        if (!format->parse(current_statement))
            return NULL;
        format->optype = RDVIEW_OPTYPE_FORMAT;
        return format;
    }

    else if (identifier == "Background")
    {
        
        rdbackground *background = new rdbackground(this);
        if (!background->parse(current_statement))
            return NULL;
        background->optype = RDVIEW_OPTYPE_BACKGROUND;
        return background;
    }

    else if (identifier == "Color")
    {
        rdcolor *color = new rdcolor(this);
        if (!color->parse(current_statement))
            return NULL;
        color->optype = RDVIEW_OPTYPE_COLOR;
        return color;
    }   

    else if (identifier == "Point")
    {
        rdpoint *point = new rdpoint(this);
        if (!point->parse(current_statement))
            return NULL;
        point->optype = RDVIEW_OPTYPE_POINT;
        return point;
    }

    else if (identifier == "Line")
    {
        rdline *line = new rdline(this);
        if (!line->parse(current_statement))
            return NULL;
        line->optype = RDVIEW_OPTYPE_LINE;
        return line;
    }

    else if (identifier == "Circle")
    {
        rdcircle *circle = new rdcircle(this);
        if (!circle->parse(current_statement))
            return NULL;
        circle->optype = RDVIEW_OPTYPE_CIRCLE;
        return circle;
    }

    else if (identifier == "Fill")
    {
        rdflood *flood = new rdflood(this);
        if (!flood->parse(current_statement))
            return NULL;
        flood->optype = RDVIEW_OPTYPE_FLOOD;
        return flood;
    }

    else if (identifier == "WorldBegin")
    {
        rdworldbegin *worldbegin = new rdworldbegin(this);
        if (!worldbegin->parse(current_statement))
            return NULL;
        worldbegin->optype = RDVIEW_OPTYPE_WORLDBEGIN;
        return worldbegin;
    }

    else if (identifier == "WorldEnd")
    {

        rdworldend *worldend = new rdworldend(this);
        if (!worldend->parse(current_statement))
            return NULL;
        worldend->optype = RDVIEW_OPTYPE_WORLDEND;
        return worldend;
    }

    else if (identifier == "CameraEye")
    {

        rdcameraeye *cameraeye = new rdcameraeye(this);
        if (!cameraeye->parse(current_statement))
            return NULL;
        cameraeye->optype = RDVIEW_OPTYPE_CAMERAEYE;
        return cameraeye;
    }

    else if (identifier == "CameraAt")
    {

        rdcameraat *cameraat = new rdcameraat(this);
        if (!cameraat->parse(current_statement))
            return NULL;
        cameraat->optype = RDVIEW_OPTYPE_CAMERAAT;
        return cameraat;
    }

    else if (identifier == "CameraUp")
    {

        rdcameraup *cameraup = new rdcameraup(this);
        if (!cameraup->parse(current_statement))
            return NULL;
        cameraup->optype = RDVIEW_OPTYPE_CAMERAUP;
        return cameraup;
    }

    else if (identifier == "Translate")
    {
        rdtranslation *translation = new rdtranslation(this);
        if (!translation->parse(current_statement))
            return NULL;
        translation->optype = RDVIEW_OPTYPE_TRANSLATION;
        return translation;
    }

    else if (identifier == "Scale")
    {
        rdscale *scale = new rdscale(this);
        if (!scale->parse(current_statement))
            return NULL;
        scale->optype = RDVIEW_OPTYPE_SCALE;
        return scale;
    }

    else if (identifier == "Cube")
    {

        rdcube *cube = new rdcube(this);
        if (!cube->parse(current_statement))
            return NULL;
        cube->optype = RDVIEW_OPTYPE_CUBE;
        return cube;
    }

    else if (identifier == "CameraFOV")
    {
        rdcamerafov *camerafov = new rdcamerafov(this);
        if (!camerafov->parse(current_statement))
            return NULL;
        camerafov->optype = RDVIEW_OPTYPE_CAMERAFOV;
        return camerafov;
    }

    else if (identifier == "XformPush")
    {
        rdxformpush *xformpush = new rdxformpush(this);
        if (!xformpush->parse(current_statement))
            return NULL;
        xformpush->optype = RDVIEW_OPTYPE_XFORMPUSH;
        return xformpush;
    }

    else if (identifier == "XformPop")
    {
        rdxformpop *xformpop = new rdxformpop(this);
        if (!xformpop->parse(current_statement))
            return NULL;
        xformpop->optype = RDVIEW_OPTYPE_XFORMPOP;
        return xformpop;
    }

    else if (identifier == "Sphere")
    {
        rdsphere *sphere = new rdsphere(this);
        if (!sphere->parse(current_statement))
            return NULL;
        sphere->optype = RDVIEW_OPTYPE_SPHERE;
        return sphere;
    }

    else if (identifier == "Rotate")
    {
        rdrotate *rotate = new rdrotate(this);
        if (!rotate->parse(current_statement))
            return NULL;
        rotate->optype = RDVIEW_OPTYPE_ROTATION;
        return rotate;
    }

    else if (identifier == "ObjectBegin")
    {
        rdobjectbegin *objectbegin = new rdobjectbegin(this);
        if (!objectbegin->parse(current_statement))
            return NULL;
        objectbegin->optype = RDVIEW_OPTYPE_OBJECTBEGIN;
        return objectbegin;
    }

    else if (identifier == "ObjectEnd")
    {
        rdobjectend *objectend = new rdobjectend(this);
        if (!objectend->parse(current_statement))
            return NULL;
        objectend->optype = RDVIEW_OPTYPE_OBJECTEND;
        return objectend;
    }

    else if (identifier == "ObjectInstance")
    {
        rdobjectinstance *instance = new rdobjectinstance(this);
        if (!instance->parse(current_statement))
            return NULL;
        instance->optype = RDVIEW_OPTYPE_OBJECTINST;
        return instance;
    }

    else if (identifier == "PolySet")
    {
        rdpolyset *poly = new rdpolyset(this);
        if (!poly->parse(current_statement))
            return NULL;
        poly->optype = RDVIEW_OPTYPE_POLYSET;
        return poly;
    }

    else if (identifier == "Cone")
    {
        rdcone *cone = new rdcone(this);
        if (!cone->parse(current_statement))
            return NULL;
        cone->optype = RDVIEW_OPTYPE_CONE;
        return cone;

    }

    else if (identifier == "Cylinder")
    {
        rdcylinder *cylinder= new rdcylinder(this);
        if (!cylinder->parse(current_statement))
            return NULL;
        cylinder->optype = RDVIEW_OPTYPE_CYLINDER;
        return cylinder;

    }

    else if (identifier == "FrameBegin")
    {
        rdframebegin *framebegin = new rdframebegin(this);
        if (!framebegin->parse(current_statement))
            return NULL;
        framebegin->optype = RDVIEW_OPTYPE_FRAMEBEGIN;
        return framebegin;

    }

    else if (identifier == "FrameEnd")
    {
        rdframeend *frameend= new rdframeend(this);
        if (!frameend->parse(current_statement))
            return NULL;
        frameend->optype = RDVIEW_OPTYPE_FRAMEEND;
        return frameend;

    }

    else if (identifier == "PointSet")
    {
        rdpointset *ps = new rdpointset(this);
        if (!ps->parse(current_statement))
            return NULL;
        ps->optype = RDVIEW_OPTYPE_POINTSET;
        return ps;

    }

    else if (identifier == "Clipping")
    {
        rdclipping *clip = new rdclipping(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_CLIPPING;
        return clip;

    }


    else if (identifier == "AmbientLight")
    {
        rdambientlight *clip = new rdambientlight(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_AMBIENTLIGHT;
        return clip;

    }

    else if (identifier == "FarLight")
    {
        rdfarlight *clip = new rdfarlight(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_FARLIGHT;
        return clip;

    }

    else if (identifier == "PointLight")
    {
        rdpointlight *clip = new rdpointlight(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_POINTLIGHT;
        return clip;

    }

    else if (identifier == "Ka")
    {
        rdka *clip = new rdka(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_KA;
        return clip;

    }

    else if (identifier == "Ks")
    {
        rdks *clip = new rdks(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_KS;
        return clip;

    }


    else if (identifier == "Kd")
    {
        rdkd *clip = new rdkd(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_KD;
        return clip;

    }

    else if (identifier == "Specular")
    {
        rdspecular *clip = new rdspecular(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_SPECULAR;
        return clip;

    }

    else if (identifier == "Surface")
    {
        rdsurface *clip = new rdsurface(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_SURFACE;
        return clip;

    }

    else if (identifier == "OptionBool")
    {
        rdoptionbool *clip = new rdoptionbool(this);
        if (!clip->parse(current_statement))
            return NULL;
        clip->optype = RDVIEW_OPTYPE_OPTIONBOOL;
        return clip;

    }

    else
    {
        current_statement->print_unknown();
        return NULL;
    }

}

bool rdview::
init()
{

    rdstatement *current_statement = NULL;
    while (this->parse_index < this->statements.size())
    {
        current_statement = &this->statements[this->parse_index];
        rdoperation *op = this->create_operation(current_statement);
        if (op != NULL) this->operations.push_back(op);
        this->parse_index++;
    }

    return true;
}

// --- RDView Parser Statements ------------------------------------------------
//
// Takes source lines and builds them out to tokens.
//

rdstatement::
rdstatement(i32 line_number, std::string line)
{

    this->number = line_number;

    std::stringstream ss { line };
    std::string current;
    while (ss >> current)
    {
        this->tokens.push_back(current);
        current = "";
    }

    rdview_parser_rejoin_strings(this->tokens);

}













#if 0
static inline void
rdview_parser_strip_whitespace(std::string &line)
{

    // Get the first non-whitespace character.
    i32 index = -1;
    for (size_t idx = 0; idx < line.length(); ++idx)
    {
        if (isspace(line[idx]))
            index = idx;
        else
            break;
    }

    if (index == -1)
        return;

    line.erase(0, index - 1);

}

static inline bool
rdview_parser_line_is_comment(std::string &line)
{
    return (line[0] == '#');
}

static inline void
rdview_parser_rejoin_strings(std::vector<std::string>& lk_line)
{

    size_t idx = 0;
    while (idx < lk_line.size())
    {

        std::string& current = lk_line[idx];
        if (current[0] == '"' && current[current.length()-1] != '"')
        {
            while (idx < lk_line.size())
            {
                current += " ";
                current += lk_line[++idx];
                lk_line.erase(lk_line.begin()+idx);
                idx--;
                if (current[current.length()-1] == '"')
                    break;
            }
        }

        idx++;

    }

}

static inline std::string
rdview_parser_keyword_dequote(std::string keyword)
{
    return keyword.substr(1, keyword.length()-2);
}

bool
rdview_source_parse(rdview_configuration *config, const char *buffer)
{

    // --- Parsing -------------------------------------------------------------
    //
    // Initial pass strips blank lines and initialize white space and then
    // collects the lines into a vector and seperates keywords and operands.
    //

    std::stringstream source_ss { buffer };
    std::vector<std::string> source_lines;
    std::string current_line;
    while (std::getline(source_ss, current_line))
    {

        // Strips white space and checks if it is a comment.
        rdview_parser_strip_whitespace(current_line);
        if (rdview_parser_line_is_comment(current_line))
            continue;

        // If the line isn't empty, its a line we need to parse.
        if (!current_line.empty())
        {
            source_lines.push_back(current_line);
        }
    }

    // Next, we need to split our lines into keywords.
    std::vector<std::vector<std::string>> line_keywords;
    for (size_t idx = 0; idx < source_lines.size(); ++idx)
    {

        std::string line = source_lines[idx];
        line_keywords.push_back({});
        std::vector<std::string>& current_lk = line_keywords.back();

        std::stringstream ss { line };
        std::string current;
        while (ss >> current)
        {
            current_lk.push_back(current);
            current = "";
        }

    }

    // Now, rejoin the strings into one single keyword.
    for (auto& lk : line_keywords)
    {
        rdview_parser_rejoin_strings(lk);
    }

    // --- Defaults ------------------------------------------------------------
    //
    // Set defaults for the configuration.
    //

    config->state.device_width  = 640;
    config->state.device_height = 480;
    config->draw_color          = { 1.0f, 1.0f, 1.0f };
    config->canvas_color        = { 0.0f, 0.0f, 0.0f };

    // --- Display -------------------------------------------------------------
    //
    // The first operation must be display.
    //

    std::vector<std::string>& display_operation = line_keywords[0];
    if (display_operation[0] != "Display")
    {
        std::cout << "Error while parsing rdview file:\n"
                  << "    Display must be the first keyword in the file." << std::endl;
        std::cout << "    Encountered \'" << display_operation[0] << "\'" << std::endl;
        return false;
    }

    else
    {

        // Ensure first that we have the correct size.
        if (display_operation.size() != 4)
        {
            std::cout << "Error while parsing rdview file:\n"
                      << "    The Display keyword requires 4 arguments.\n"
                      << "    Display \"[name]\" \"[type]\" \"[mode]\"" << std::endl;
            return false;
        }
        
        // The display basically tells us what device and system we should use.
        std::string name = rdview_parser_keyword_dequote(display_operation[1]);
        std::string type = rdview_parser_keyword_dequote(display_operation[2]);
        std::string mode = rdview_parser_keyword_dequote(display_operation[3]);

        // Copy name.
        memcpy(config->state.display_name, name.c_str(), name.length());

        // Check type.
        if (type == "Screen")
            config->state.device_type = RDVIEW_DEVICE_WINDOW;
        else if (type == "PNM")
            config->state.device_type = RDVIEW_DEVICE_PNM;
        else
        {
            std::cout   << "Error while parsing rdview file:\n"
                        << "    Display type \"" << type << "\" is unknown.\n"
                        << "    Valid display types are \"Screen\" and \"PNM\"." << std::endl;
            return false;
        }

        // Check mode.
        if (type == "Screen")
        {
            if (mode == "rgbsingle")
            {
                config->state.device_draw_method = RDVIEW_DRAW_TYPE_SINGLE;
            }
            else if (mode == "rgbdouble")
            {
                config->state.device_draw_method = RDVIEW_DRAW_TYPE_DOUBLE;
            }
            else if (mode == "rgbobject")
            {
                config->state.device_draw_method = RDVIEW_DRAW_TYPE_STEP;
            }
            else
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Display mode \"" << type << "\" is unknown." << std::endl;
                return false;
            }
        }

        else if (type == "PNM")
        {
            if (mode == "rgb")
            {
                config->state.device_draw_method = RDVIEW_DRAW_TYPE_OUTPUT;
            }
            else
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Display mode \"" << type << "\" is unknown."
                            << "    PNM only supports \"rgb\" as a display mode." << std::endl;
                return false;

            }
        }

    }

    // --- Pre-Generation Keywords ---------------------------------------------
    //
    // All keywords that come before FrameBegin & WorldBegin invocations.
    //

    size_t line_idx = 1;
    for (; line_idx < line_keywords.size(); ++line_idx)
    {
        
        // Get the current line. If the current line is FrameBegin or WorldBegin,
        // then we know we are about to generate some render stuff.
        std::vector<std::string> current_line = line_keywords[line_idx];
        if (current_line[0] == "FrameBegin" || current_line[0] == "WorldBegin")
            break;

        if (current_line[0] == "Format")
        {

            // Validate that point is correctly formated.
            if (current_line.size() != 3)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Format doesn't contain the current number of arguments.\n"
                            << "    Argument format is \"Format [width] [height]\"" << std::endl;
                return false;
            }

            config->state.device_width = std::stoi(current_line[1]);
            config->state.device_height = std::stoi(current_line[2]);

            if (config->state.device_type == RDVIEW_DEVICE_WINDOW && config->state.device_width < 240)
            {
                std::cout   << "Warning while parsing rdview file:\n"
                            << "    The format size is too small for device type \"Display\".\n"
                            << "    Setting width to 240." << std::endl;
                config->state.device_width = 240;
            }

            if (config->state.device_type == RDVIEW_DEVICE_WINDOW && config->state.device_height < 200)
            {
                std::cout   << "Warning while parsing rdview file:\n"
                            << "    The format size is too small for device type \"Display\".\n"
                            << "    Setting height to 200." << std::endl;
                config->state.device_height = 200;
            }

        }
        else
        {

            std::cout   << "Warning while parsing rdview file:\n"
                        << "    Keyword \"" << current_line[0] << " is an unrecognized symbol.\n"
                        << "    This keyword has been ignore." << std::endl;
        }

    };

    // --- Generation Keywords -------------------------------------------------
    //
    // Now, we have only generational keywords to process. Operands are stored
    // as a singly-linked list which can be consumed in a sequence. The first
    // node is always the "root".
    //

    config->operation_list = (rdview_operation*)malloc(sizeof(rdview_operation));
    config->operation_list->type            = RDVIEW_OPTYPE_ROOT;
    config->operation_list->operands        = NULL;
    config->operation_list->next_operation  = NULL;

    rdview_operation *current_operation = config->operation_list;

    for (; line_idx < line_keywords.size(); ++line_idx)
    {
        
        std::vector<std::string> current_line = line_keywords[line_idx];
        if (current_line[0] == "WorldEnd")
            break;

        if (current_line[0] == "FrameBegin")
        {
            std::cout   << "Warning while parsing rdview file:\n"
                        << "    FrameBegin is not implemented yet." << std::endl;
        }

        else if (current_line[0] == "WorldBegin")
            continue;

        else if (current_line[0] == "Color")
        {
            
            // Validate that point is correctly formated.
            if (current_line.size() != 4)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Color doesn't contain the current number of arguments." << std::endl;
                return false;
            }

            // Bump the current operation.
            current_operation->next_operation = (rdview_operation*)malloc(sizeof(rdview_operation));
            current_operation = current_operation->next_operation;
            current_operation->next_operation = NULL;
            current_operation->type = RDVIEW_OPTYPE_COLOR;
            
            rdview_color *color = (rdview_color*)malloc(sizeof(rdview_color));
            color->r = std::stof(current_line[1]);
            color->g = std::stof(current_line[2]);
            color->b = std::stof(current_line[3]);
            current_operation->operands = color;

        }

        else if (current_line[0] == "Background")
        {
            
            // Validate that point is correctly formated.
            if (current_line.size() != 4)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Background doesn't contain the current number of arguments." << std::endl;
                return false;
            }

            // Bump the current operation.
            current_operation->next_operation = (rdview_operation*)malloc(sizeof(rdview_operation));
            current_operation = current_operation->next_operation;
            current_operation->next_operation = NULL;
            current_operation->type = RDVIEW_OPTYPE_CANVAS;
            
            rdview_color *color = (rdview_color*)malloc(sizeof(rdview_color));
            color->r = std::stof(current_line[1]);
            color->g = std::stof(current_line[2]);
            color->b = std::stof(current_line[3]);
            current_operation->operands = color;

        }

        else if (current_line[0] == "Point")
        {

            // Validate that point is correctly formated.
            if (current_line.size() != 4)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Point doesn't contain the current number of arguments." << std::endl;
                return false;
            }

            // Bump the current operation.
            current_operation->next_operation = (rdview_operation*)malloc(sizeof(rdview_operation));
            current_operation = current_operation->next_operation;
            current_operation->next_operation = NULL;
            current_operation->type = RDVIEW_OPTYPE_POINT;

            // Set the operands.
            rdview_point *point = (rdview_point*)malloc(sizeof(rdview_point));
            point->x = std::stoi(current_line[1]);
            point->y = std::stoi(current_line[2]);
            point->z = std::stoi(current_line[3]);
            current_operation->operands = point;

        }

        else if (current_line[0] == "Line")
        {

            // Validate that point is correctly formated.
            if (current_line.size() != 7)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Line doesn't contain the current number of arguments." << std::endl;
                return false;
            }

            // Bump the current operation.
            current_operation->next_operation = (rdview_operation*)malloc(sizeof(rdview_operation));
            current_operation = current_operation->next_operation;
            current_operation->next_operation = NULL;
            current_operation->type = RDVIEW_OPTYPE_LINE;

            // Set the operands.
            rdview_line *line = (rdview_line*)malloc(sizeof(rdview_line));
            line->x1 = std::stoi(current_line[1]);
            line->y1 = std::stoi(current_line[2]);
            line->z1 = std::stoi(current_line[3]);
            line->x2 = std::stoi(current_line[4]);
            line->y2 = std::stoi(current_line[5]);
            line->z2 = std::stoi(current_line[6]);
            current_operation->operands = line;

        }

        else if (current_line[0] == "Circle")
        {
            
            // Validate that point is correctly formated.
            if (current_line.size() != 5)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Circle doesn't contain the current number of arguments." << std::endl;
                return false;
            }

            // Bump the current operation.
            current_operation->next_operation = (rdview_operation*)malloc(sizeof(rdview_operation));
            current_operation = current_operation->next_operation;
            current_operation->next_operation = NULL;
            current_operation->type = RDVIEW_OPTYPE_CIRCLE;

            // Set the operands.
            rdview_circle *circle = (rdview_circle*)malloc(sizeof(rdview_circle));
            circle->x = std::stoi(current_line[1]);
            circle->y = std::stoi(current_line[2]);
            circle->z = std::stoi(current_line[3]);
            circle->r = std::stoi(current_line[4]);
            current_operation->operands = circle;
        }

        else if (current_line[0] == "Fill")
        {

            // Validate that point is correctly formated.
            if (current_line.size() != 4)
            {
                std::cout   << "Error while parsing rdview file:\n"
                            << "    Fill doesn't contain the current number of arguments." << std::endl;
                return false;
            }

            // Bump the current operation.
            current_operation->next_operation = (rdview_operation*)malloc(sizeof(rdview_operation));
            current_operation = current_operation->next_operation;
            current_operation->next_operation = NULL;
            current_operation->type = RDVIEW_OPTYPE_FLOOD;

            // Set the operands.
            rdview_flood *flood = (rdview_flood*)malloc(sizeof(rdview_flood));
            flood->x = std::stoi(current_line[1]);
            flood->y = std::stoi(current_line[2]);
            flood->z = std::stoi(current_line[3]);
            current_operation->operands = flood;

        }

        else
        {
            std::cout   << "Warning while parsing rdview file:\n"
                        << "    Generational keyword \"" << current_line[0]
                        << "\" is not a recognized keyword/or unimplemented." << std::endl;
        }

    };


    return true;

}

void
rdview_source_run(rdview_configuration *config)
{

    // First operation is always the root operation.
    assert(config->operation_list->type == RDVIEW_OPTYPE_ROOT);

    rdview_operation *current_op = config->operation_list->next_operation;
    while (current_op != NULL)
    {
    
        // Point.
        if (current_op->type == RDVIEW_OPTYPE_POINT)
        {

            rdview_point *point = (rdview_point*)current_op->operands;
            set_pixel(point->x, point->y, config->draw_color);

        }

        // Line.
        else if (current_op->type == RDVIEW_OPTYPE_LINE)
        {
    
            rdview_line *line = (rdview_line*)current_op->operands;
            set_line(line->x1, line->x2, line->y1, line->y2, line->z1, line->z2,
                    config->draw_color);

        }

        // Circle.
        else if (current_op->type == RDVIEW_OPTYPE_CIRCLE)
        {
            rdview_circle *circle = (rdview_circle*)current_op->operands;
            set_circle(circle->x, circle->y, circle->z, circle->r, config->draw_color);
        }

        else if (current_op->type == RDVIEW_OPTYPE_FLOOD)
        {
            rdview_flood *flood= (rdview_flood*)current_op->operands;
            set_flood(flood->x, flood->y, flood->z, config->draw_color);

        }

        // Canvas.
        else if (current_op->type == RDVIEW_OPTYPE_CANVAS)
        {
            
            rdview_color *color = (rdview_color*)current_op->operands;
            config->canvas_color = { color->r, color->g, color->b };
            set_fill(config->canvas_color);

        }

        // Color.
        else if (current_op->type == RDVIEW_OPTYPE_COLOR)
        {
            rdview_color *color = (rdview_color*)current_op->operands;
            config->draw_color = { color->r, color->g, color->b };
        }

        current_op = current_op->next_operation;

    }

}
#endif
