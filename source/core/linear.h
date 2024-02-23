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

    inline v2& operator+=(const v2& rhs);
    inline v2& operator*=(const f32& rhs);

};

inline v2& v2::
operator*=(const f32& rhs)
{
    this->x *= rhs;
    this->y *= rhs;
    return *this;
}

inline v2& v2::
operator+=(const v2& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

inline v2
operator*(const v2& lhs, const f32& rhs)
{
    v2 res = lhs;
    res.x *= rhs;
    res.y *= rhs;
    return res;
}

inline v2
operator+(const v2& lhs, const v2& rhs)
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

    inline v3& operator+=(const v3& rhs);
    inline v3& operator*=(const f32& rhs);

};

inline v3& v3::
operator*=(const f32& rhs)
{
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    return *this;
}

inline v3& v3::
operator+=(const v3& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}

inline v3
operator*(const v3& lhs, const f32& rhs)
{
    v3 res = lhs;
    res.x *= rhs;
    res.y *= rhs;
    res.z *= rhs;
    return res;
}

inline v3
operator+(const v3& lhs, const v3& rhs)
{
    v3 res = {};
    res.x = lhs.x + rhs.x;
    res.y = lhs.y + rhs.y;
    res.z = lhs.z + rhs.z;
    return res;
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

    inline v4& operator+=(const v4& rhs);
    inline v4& operator*=(const f32& rhs);

};

inline v4& v4::
operator*=(const f32& rhs)
{
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    this->w *= rhs;
    return *this;
}

inline v4& v4::
operator+=(const v4& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    this->w += rhs.w;
    return *this;
}

inline v4
operator*(const v4& lhs, const f32& rhs)
{
    v4 res = lhs;
    res.x *= rhs;
    res.y *= rhs;
    res.z *= rhs;
    res.w *= rhs;
    return res;
}

inline v4
operator+(const v4& lhs, const v4& rhs)
{
    v4 res = {};
    res.x = lhs.x + rhs.x;
    res.y = lhs.y + rhs.y;
    res.z = lhs.z + rhs.z;
    res.w = lhs.w + rhs.w;
    return res;
};

#endif
