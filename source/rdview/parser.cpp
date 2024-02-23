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

char *
rdview_source_fetch(const char *file_path)
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

void
rdview_source_free(char *buffer)
{
    free(buffer);
}

// --- The Parser --------------------------------------------------------------
//
// This is going to be scuffed, guaranteed.
//

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
