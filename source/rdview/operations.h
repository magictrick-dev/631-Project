#ifndef RDVIEW_OPERATIONS_H
#define RDVIEW_OPERATIONS_H
#include <core.h>
#include <string>
#include <vector>
#include <renderer/renderer.h>

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

// --- RD Objects --------------------------------------------------------------
//
// Contains routines for objects.
//

class rdobject
{
    public:
        inline void run() { for (rdoperation* op : operations) op->execute(); };

        std::string name;
        std::vector<rdoperation*> operations;
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

class rdrotate : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdrotate(void *parent);

    i32 axis;
    f32 theta;

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

class rdxformpush : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdxformpush(void *parent);
};

class rdxformpop : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdxformpop(void *parent);
};

class rdsphere : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdsphere(void *parent);

    f32 zmin;
    f32 zmax;
    f32 r;
    f32 theta;

};

class rdobjectbegin : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdobjectbegin(void *parent);

        rdobject    object;
        size_t      parse_index;

};

class rdobjectend : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdobjectend(void *parent);
};

class rdobjectinstance : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdobjectinstance(void *parent);

    std::string object_name;
    rdobject *object = NULL;

};

class rdpolyset : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdpolyset(void *parent);

        std::string type;
        i32 verts;
        i32 fc;
        std::vector<attr_point> points;
        std::vector<std::vector<i32>> faces;
};

class rdcone : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcone(void *parent);

        f32 h;
        f32 r;
        f32 theta;
};

class rdcylinder : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdcylinder(void *parent);

    f32 r;
    f32 zmin;
    f32 zmax;
    f32 theta;
};

class rdframebegin : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdframebegin(void *parent);

        i32             frame_number;
        rdobject        operations;
};

class rdframeend : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdframeend(void *parent);
};

class rdpointset : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdpointset(void *parent);

        std::string type;
        i32 verts;
        std::vector<v3> points;
};

class rdclipping : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdclipping(void *parent);

        f32 nearp;
        f32 farp;
};

class rdfarlight : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdfarlight(void *parent);

        farlight        light;
};

class rdambientlight : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdambientlight(void *parent);

        ambientlight    light;
};

class rdpointlight : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdpointlight(void *parent);

        pointlight      light;
};

class rdka : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdka(void *parent);

        f32 Ka;
};

class rdkd : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdkd(void *parent);

        f32 Kd;
};

class rdks : public rdoperation
{
    public:
        virtual void    execute();
        virtual bool    parse(void *statement);
                        rdks(void *parent);

        f32 Ks;
};

#endif
