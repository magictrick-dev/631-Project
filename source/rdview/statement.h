#ifndef RDVIEW_STATEMENT_H
#define RDVIEW_STATEMENT_H
#include <iostream>
#include <string>
#include <core.h>
#include <vector>

// --- RDStatements ------------------------------------------------------------

class rdstatement
{
    public:
        rdstatement(i32 line_number, std::string line);

        inline std::string  operator[](i32 idx) const { return this->tokens[idx]; };
        inline size_t       count() const { return this->tokens.size(); };

        inline i32          get_line_number() const { return this->number; };
        inline std::string  get_identifier() const { return this->tokens[0]; };

        inline void         print_unknown() const
        {
            std::cout << "Warning(" << this->number << "):\n"
                << "    Token: " << this->tokens[0] << " is an unrecognized symbol."
                << std::endl;
            std::cout << "    ";
            for (size_t i = 0; i < tokens.size(); ++i)
                std::cout << tokens[i] << " ";
            std::cout << std::endl;
        }

        inline void         print_error(std::string e) const
        {
            std::cout << "Error(" << this->number << "): \n"
                << "    " << e << std::endl;
        }

        inline void         print_error(std::vector<std::string> es) const
        {
            std::cout << "Error(" << this->number << "):\n";
            for (auto e : es)
                std::cout << "    " << e << std::endl;
        }

    protected:
        i32 number;
        std::vector<std::string> tokens;
};

#endif
