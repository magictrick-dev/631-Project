#define LINEAR_DEFINE_OSTREAM
#include <core/primitives.h>

struct v2;
struct v3;
struct v4;

// --- Vector 2D ---------------------------------------------------------------
//
// Forms the basis of a general vector-2D.
//

struct v2
{
    
    union
    {
        f32 elements[2];

        struct
        {
            f32 x;
            f32 y;
        };

        struct
        {
            f32 width;
            f32 height;
        };

    };

    inline v2&      operator*=(const f32& rhs);
    inline v2&      operator+=(const v2&  rhs);
    inline v2&      operator-=(const v2&  rhs);

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

inline v2& v2::
operator-=(const v2& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

inline v2
operator*(const v2& lhs, const f32& rhs)
{
    v2 result = lhs;
    result *= rhs;
    return result;
}

inline v2
operator*(const f32& lhs, const v2& rhs)
{
    v2 result = rhs;
    result *= lhs;
    return result;
}

inline v2
operator+(const v2& lhs, const v2& rhs)
{
    v2 result = lhs;
    result += rhs;
    return result;
}

inline v2
operator-(const v2& lhs, const v2& rhs)
{
    v2 result = lhs;
    result -= rhs;
    return result;
}

#ifdef LINEAR_DEFINE_OSTREAM
#include <iostream>
std::ostream&
operator<<(std::ostream& out, const v2& v)
{
    out << "{ " << v.x << ", " << v.y << " }";
    return out;
}
#endif

// --- Vector 3D ---------------------------------------------------------------
//
//
//

struct v3
{
    
    union
    {
        f32 elements[3];

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
            v2 yz;
        };

    };

    inline v3&      operator*=(const f32& rhs);
    inline v3&      operator+=(const v3&  rhs);
    inline v3&      operator-=(const v3&  rhs);

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

inline v3& v3::
operator-=(const v3& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}

inline v3
operator*(const v3& lhs, const f32& rhs)
{
    v3 result = lhs;
    result *= rhs;
    return result;
}

inline v3
operator*(const f32& lhs, const v3& rhs)
{
    v3 result = rhs;
    result *= lhs;
    return result;
}

inline v3
operator+(const v3& lhs, const v3& rhs)
{
    v3 result = lhs;
    result += rhs;
    return result;
}

inline v3
operator-(const v3& lhs, const v3& rhs)
{
    v3 result = lhs;
    result -= rhs;
    return result;
}

#ifdef LINEAR_DEFINE_OSTREAM
#include <iostream>
std::ostream&
operator<<(std::ostream& out, const v3& v)
{
    out << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
    return out;
}
#endif


// --- Vector 4D ---------------------------------------------------------------
//
//
//

struct v4
{
    
    union
    {
        f32 elements[4];

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
            v2 xyz;
            f32 _d0;
        };

        struct
        {
            v2 xy;
            v2 zw;
        };

        struct
        {
            f32 _d1;
            v2 yzw;
        };

        struct
        {
            f32 _d2;
            v2 yz;
            f32 _d3;
        };

    };

    inline v4&      operator*=(const f32& rhs);
    inline v4&      operator+=(const v4&  rhs);
    inline v4&      operator-=(const v4&  rhs);

};

inline v4& v4::
operator*=(const f32& rhs)
{
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    return *this;
}

inline v4& v4::
operator+=(const v4& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}

inline v4& v4::
operator-=(const v4& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}

inline v4
operator*(const v4& lhs, const f32& rhs)
{
    v4 result = lhs;
    result *= rhs;
    return result;
}

inline v4
operator*(const f32& lhs, const v4& rhs)
{
    v4 result = rhs;
    result *= lhs;
    return result;
}

inline v4
operator+(const v4& lhs, const v4& rhs)
{
    v4 result = lhs;
    result += rhs;
    return result;
}

inline v4
operator-(const v4& lhs, const v4& rhs)
{
    v4 result = lhs;
    result -= rhs;
    return result;
}

#ifdef LINEAR_DEFINE_OSTREAM
#include <iostream>
std::ostream&
operator<<(std::ostream& out, const v4& v)
{
    out << "{ " << v.x << ", " << v.y << ", "
        << v.z << ", " << v.w << " }";
    return out;
}
#endif

// --- Matrix 4x4 --------------------------------------------------------------
//
// Forms the basis of a 4x4 matrix.
//

struct m4
{
    
    union
    {
        f32 elements[16];

        struct
        {
            v4 rows[4];
        };

        struct
        {
            v4 r1;
            v4 r2;
            v4 r3;
            v4 r4;
        };

    };

    inline f32& operator[](size_t index);

};

inline f32& m4::
operator[](size_t index)
{
    return elements[index];
}

#ifdef LINEAR_DEFINE_OSTREAM
#include <iostream>
std::ostream&
operator<<(std::ostream& out, const m4& m)
{

    std::cout << "{ ";

    for (size_t row = 0; row < 4; ++row)
    {
        
        std::cout << "{ " << m.rows[row].x << ", " <<
            m.rows[row].y << ", " << m.rows[row].z << ", " <<
            m.rows[row].w << " }";

        if (row < 3)
            std::cout << ", ";

    };

    std::cout << " }";

    return out;
}
#endif


