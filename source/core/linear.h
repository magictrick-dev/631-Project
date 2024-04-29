#ifndef J5_CORE_LINEAR_H
#define J5_CORE_LINEAR_H
#define LINEAR_DEFINE_OSTREAM
#include <core/primitives.h>

struct v3;
struct v4;
struct m4;

v4 parameterize(v4 a, v4 b, f32 t);
v4 homogenize(v4 h);

inline f32 clamp(f32 a, f32 c, f32 b)
{
    if (c < a)
        return a;
    else if (c > b)
        return b;
    else
        return c;
}

// --- Vector 3D ---------------------------------------------------------------

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

    };

    v3&          operator*=(const f32& rhs);
    v3&          operator+=(const v3&  rhs);
    v3&          operator-=(const v3&  rhs);
    f32&         operator[](size_t idx);
    const f32&   operator[](size_t idx) const;

};

v3 operator*(const v3& lhs, const f32& rhs);
v3 operator*(const f32& lhs, const v3& rhs);
v3 operator+(const v3& lhs, const v3& rhs);
v3 operator-(const v3& lhs, const v3& rhs);

// --- Vector 4D ---------------------------------------------------------------

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
            v3 xyz;
            f32 _d0;
        };

        struct
        {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };

    };

    v4&         operator*=(const f32& rhs);
    v4&         operator+=(const v4&  rhs);
    v4&         operator-=(const v4&  rhs);
    f32&        operator[](size_t idx);
    const f32&  operator[](size_t idx) const;

    f32         magnitude_squared() const;
    v4          normalize() const;

};

v4      normalize(const v4& vector);
f32     magnitude_squared(const v4& vector);
v4      cross(const v4& lhs, const v4& rhs);
f32     dot(const v4& lhs, const v4& rhs);

v4      operator*(const v4& lhs, const f32& rhs);
v4      operator*(const f32& lhs, const v4& rhs);
v4      operator+(const v4& lhs, const v4& rhs);
v4      operator-(const v4& lhs, const v4& rhs);

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

    f32& operator[](size_t index);
    const f32& operator[](size_t index) const;

    static m4 create_zero();
    static m4 create_identity();
    static m4 create_transform(v3 t);
    static m4 create_transformi(v3 t);
    static m4 create_scale(v3 s);
    static m4 create_scalei(v3 s);
    static m4 create_rotation(v3 r);
    static m4 create_rotation_z(f32 degrees);
    static m4 create_rotation_y(f32 degrees);
    static m4 create_rotation_x(f32 degrees);
    static m4 create_rotation_zi(f32 degrees);
    static m4 create_rotation_yi(f32 degrees);
    static m4 create_rotation_xi(f32 degrees);
    static m4 create_world_to_camera(v4 eye, v4 at, v4 up);
    static m4 create_camera_to_clip(f32 fov, f32 near, f32 far, f32 aspect_ratio);
    static m4 create_clip_to_device(int width, int height);

    m4& operator*=(const m4& rhs);

};

v4 operator*(const m4& lhs, const v4& rhs);
m4 operator*(const m4& lhs, const m4& rhs);

// --- Output Stream Overloads -------------------------------------------------

#ifdef LINEAR_DEFINE_OSTREAM
#include <iostream>
std::ostream& operator<<(std::ostream& out, const v3& v);
std::ostream& operator<<(std::ostream& out, const v4& v);
std::ostream& operator<<(std::ostream& out, const m4& m);
#endif

#endif
