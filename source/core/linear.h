#ifndef J5_LINEAR_MATH_H
#define J5_LINEAR_MATH_H
#include <core/primitives.h>

struct v2
{

    union
    {
        f32 c[2];

        struct
        {
            f32 x;
            f32 y;
        };

        struct
        {
            f32 u;
            f32 v;
        };

        struct
        {
            f32 width;
            f32 height;
        };

    };

    // --- Addition Operations -------------------------------------------------

    inline v2& operator+(const v2& rhs);

};

v2& v2::
operator+(const v2& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return (*this);
};

inline v2
operator+(const v2& rhs, const v2& lhs)
{
    v2 res = {};
    res.x = lhs.x + rhs.x;
    res.y = lhs.y + rhs.y;
    return res;
};

struct v3
{
    
    union
    {

        f32 c[3];

        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };

        struct
        {
            f32 r;
            f32 g;
            f32 b;
        };

        struct
        {
            v2 xy;
            f32 _d0;
        };

        struct
        {
            f32 _d1;
            f32 yz;
        };

    };

};

struct v4
{

    union
    {
        f32 c[4];

        struct
        {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };

        struct
        {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };

        struct
        {
            v3 xyz;
            f32 _d0;
        };

        struct
        {
            f32 _d1;
            v3 yzw;
        };

        struct
        {
            v3 rgb;
            f32 _d2;
        };

        struct
        {
            v2 xy;
            v2 zw;
        };

        struct
        {
            f32 _d3;
            v2 yz;
            f32 _d4;
        };

    };

};

#endif
