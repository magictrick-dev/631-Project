#ifndef RDVIEW_OPERATIONS_H
#define RDVIEW_OPERATIONS_H
#include <core.h>
#include <string>

// --- Base Operation Class ----------------------------------------------------
//
// Contains all the base behavior for operations.
//

class rdoperation
{
    public:
        virtual void    execute()               = NULL;
        virtual bool    parse(void *statement)  = NULL;

        u32             optype                  = NULL;
        void           *rdview_parent           = NULL;
};

// --- Operations --------------------------------------------------------------
//
// This isn't the best way to do this, but it's the way I am doing it.
//

class rddisplay : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rddisplay(void *parent);

        std::string     title;
        u32             device;
        u32             mode;
};

class rdformat : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdformat(void *parent);

        i32 width;
        i32 height;
};

class rdbackground : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdbackground(void *parent);

        v3 color;
};

class rdcolor : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcolor(void *parent);

        v3 color;
};

class rdpoint : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdpoint(void *parent);

        i32 x;
        i32 y;
        i32 z;
};

class rdline : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdline(void *parent);

        i32 x1;
        i32 x2;
        i32 y1;
        i32 y2;
        i32 z1;
        i32 z2;

};

class rdcircle : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcircle(void *parent);

        i32 x;
        i32 y;
        i32 z;
        i32 r;
};

class rdflood : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdflood(void *parent);

        i32 x;
        i32 y;
        i32 z;
};

class rdworldbegin : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdworldbegin(void *parent);
};

class rdworldend : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdworldend(void *parent);
};
#endif
