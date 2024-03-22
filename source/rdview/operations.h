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

        f32 x;
        f32 y;
        f32 z;
};

class rdline : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdline(void *parent);

        f32 x1;
        f32 x2;
        f32 y1;
        f32 y2;
        f32 z1;
        f32 z2;

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

class rdcameraeye : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcameraeye(void *parent);

        v3 eye;
};

class rdcameraat : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcameraat(void *parent);

        v3 at;
};

class rdcameraup : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcameraup(void *parent);

        v3 up;
};

class rdtranslation : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdtranslation(void *parent);

        f32 x;
        f32 y;
        f32 z;
};

class rdscale : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdscale(void *parent);

        f32 x;
        f32 y;
        f32 z;
};

class rdcube : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcube(void *parent);
};

class rdcamerafov : public rdoperation
{

    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcamerafov(void *parent);

    f32 fov;

};

#endif
