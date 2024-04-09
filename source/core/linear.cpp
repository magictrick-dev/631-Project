#include <core/linear.h>
#include <cmath>

v4 homogenize(v4 h)
{
    h.x /= h.w;
    h.y /= h.w;
    h.z /= h.w;
    h.w /= h.w;
    return h;
}

// --- Vector 3D ---------------------------------------------------------------

const f32& v3::
operator[](size_t idx) const
{

    return this->elements[idx];

}


f32& v3::
operator[](size_t idx)
{

    return this->elements[idx];

}

v3& v3::
operator*=(const f32& rhs)
{
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    return *this;
}

v3& v3::
operator+=(const v3& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}

v3& v3::
operator-=(const v3& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}

v3
operator*(const v3& lhs, const f32& rhs)
{
    v3 result = lhs;
    result *= rhs;
    return result;
}

v3
operator*(const f32& lhs, const v3& rhs)
{
    v3 result = rhs;
    result *= lhs;
    return result;
}

v3
operator+(const v3& lhs, const v3& rhs)
{
    v3 result = lhs;
    result += rhs;
    return result;
}

v3
operator-(const v3& lhs, const v3& rhs)
{
    v3 result = lhs;
    result -= rhs;
    return result;
}

// --- Vector 4D ---------------------------------------------------------------

f32& v4::
operator[](size_t idx)
{
    return this->elements[idx];
}

const f32& v4::
operator[](size_t idx) const
{
    return this->elements[idx];
}


v4& v4::
operator*=(const f32& rhs)
{
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    return *this;
}

v4& v4::
operator+=(const v4& rhs)
{

    

    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;

    

    return *this;
}

v4& v4::
operator-=(const v4& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}

v4
operator*(const v4& lhs, const f32& rhs)
{
    v4 result = lhs;
    result *= rhs;
    return result;
}

v4
operator*(const f32& lhs, const v4& rhs)
{
    v4 result = rhs;
    result *= lhs;
    return result;
}

v4
operator+(const v4& lhs, const v4& rhs)
{
    v4 result = lhs;
    result += rhs;
    return result;
}

v4
operator-(const v4& lhs, const v4& rhs)
{
    v4 result = lhs;
    result -= rhs;
    return result;
}

f32 v4::
magnitude_squared() const
{
    return (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
}

v4 v4::
normalize() const
{
    v4 result = (*this);
    f32 magnitude = sqrtf(result.magnitude_squared());
    result.x /= magnitude;
    result.y /= magnitude;
    result.z /= magnitude;
    return result;

}

v4
normalize(const v4& vector)
{
    return vector.normalize();
}

f32
magnitude(const v4& vector)
{
    return vector.magnitude_squared();
}

f32
dot(const v4& lhs, const v4& rhs)
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

v4
cross(const v4& lhs, const v4& rhs)
{

    v4 result = { 0.0f, 0.0f, 0.0f, 1.0f };
    result.x = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]);
    result.y = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]);
    result.z = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0]);
    return result;

}

// --- Matrix 4D ---------------------------------------------------------------

f32& m4::
operator[](size_t index)
{
    return elements[index];
}

const f32& m4::
operator[](size_t index) const
{
    return elements[index];
}

m4& m4::
operator*=(const m4& rhs)
{
    m4 result = m4::create_zero();
    for (size_t r = 0; r < 4; ++r)
    {

        result.rows[r][0] = (this->rows[r][0] * rhs.rows[0][0]) +
                            (this->rows[r][1] * rhs.rows[1][0]) +
                            (this->rows[r][2] * rhs.rows[2][0]) +
                            (this->rows[r][3] * rhs.rows[3][0]);

        result.rows[r][1] = (this->rows[r][0] * rhs.rows[0][1]) +
                            (this->rows[r][1] * rhs.rows[1][1]) +
                            (this->rows[r][2] * rhs.rows[2][1]) +
                            (this->rows[r][3] * rhs.rows[3][1]);

        result.rows[r][2] = (this->rows[r][0] * rhs.rows[0][2]) +
                            (this->rows[r][1] * rhs.rows[1][2]) +
                            (this->rows[r][2] * rhs.rows[2][2]) +
                            (this->rows[r][3] * rhs.rows[3][2]);

        result.rows[r][3] = (this->rows[r][0] * rhs.rows[0][3]) +
                            (this->rows[r][1] * rhs.rows[1][3]) +
                            (this->rows[r][2] * rhs.rows[2][3]) +
                            (this->rows[r][3] * rhs.rows[3][3]);
    }

    *this = result;

    return (*this);

}

m4
operator*(const m4& lhs, const m4& rhs)
{
    m4 result = lhs;
    result *= rhs;
    return result;
}

m4 m4::
create_zero()
{
    m4 zero;
    zero.r1 = { 0.0f, 0.0f, 0.0f, 0.0f };
    zero.r2 = { 0.0f, 0.0f, 0.0f, 0.0f };
    zero.r3 = { 0.0f, 0.0f, 0.0f, 0.0f };
    zero.r4 = { 0.0f, 0.0f, 0.0f, 0.0f };
    return zero;
}

m4 m4::
create_identity()
{

    m4 identity;
    identity.r1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    identity.r2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    identity.r3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    identity.r4 = { 0.0f, 0.0f, 0.0f, 1.0f };
    return identity;

}

m4 m4::
create_transform(v3 t)
{

    m4 transform = m4::create_identity();
    transform.rows[0][3] = t[0];
    transform.rows[1][3] = t[1];
    transform.rows[2][3] = t[2];
    return transform;

}

m4 m4::
create_rotation_z(f32 degrees)
{

    m4 result = m4::create_identity();
    result.rows[0][0] =  cosf(DEGREES_TO_RADIANS(degrees));
    result.rows[0][1] = -sinf(DEGREES_TO_RADIANS(degrees));
    result.rows[1][0] =  sinf(DEGREES_TO_RADIANS(degrees));
    result.rows[1][1] =  cosf(DEGREES_TO_RADIANS(degrees));
    return result;

}

m4 m4::
create_rotation_y(f32 degrees)
{

    m4 result = m4::create_identity();
    result.rows[0][0] =  cosf(DEGREES_TO_RADIANS(degrees));
    result.rows[0][2] =  sinf(DEGREES_TO_RADIANS(degrees));
    result.rows[2][0] = -sinf(DEGREES_TO_RADIANS(degrees));
    result.rows[2][2] =  cosf(DEGREES_TO_RADIANS(degrees));
    return result;

}

m4 m4::
create_rotation_x(f32 degrees)
{

    m4 result = m4::create_identity();
    result.rows[1][1] =  cosf(DEGREES_TO_RADIANS(degrees));
    result.rows[1][2] = -sinf(DEGREES_TO_RADIANS(degrees));
    result.rows[2][1] =  sinf(DEGREES_TO_RADIANS(degrees));
    result.rows[2][2] =  cosf(DEGREES_TO_RADIANS(degrees));
    return result;

}


m4 m4::
create_rotation(v3 r)
{

    m4 z = m4::create_rotation_z(r.z);
    m4 y = m4::create_rotation_y(r.y);
    m4 x = m4::create_rotation_y(r.x);

    m4 result = z * y * x;
    return result;

}

m4 m4::
create_scale(v3 s)
{
    m4 scale = m4::create_identity();
    scale.rows[0][0] = s[0];
    scale.rows[1][1] = s[1];
    scale.rows[2][2] = s[2];
    return scale;
}

m4 m4::
create_world_to_camera(v4 eye, v4 at, v4 up)
{

    // A.
    v4 A = at - eye;
    A = A.normalize();

    // U.
    v4 U = cross(A, up);
    U = U.normalize();

    // V.
    v4 V = cross(U, A);
    V = V.normalize();

    m4 R = m4::create_identity();
    R.rows[0][3] = -eye.x;
    R.rows[1][3] = -eye.y;
    R.rows[2][3] = -eye.z;
    R.rows[3][3] = 1.0f;

    m4 T = m4::create_zero();
    T.rows[3][3] = 1.0f;
    T.rows[0][0] = U.x;
    T.rows[0][1] = U.y;
    T.rows[0][2] = U.z;
    T.rows[1][0] = V.x;
    T.rows[1][1] = V.y;
    T.rows[1][2] = V.z;
    T.rows[2][0] = A.x;
    T.rows[2][1] = A.y;
    T.rows[2][2] = A.z;

    m4 result = T * R;
    return result;

}

m4 m4::
create_camera_to_clip(f32 fov, f32 n, f32 f, f32 a)
{

    f32 tf = tanf(DEGREES_TO_RADIANS(fov) / 2);
    m4 T = m4::create_zero();
    T.rows[0][0] = 1.0f / (a * tf);
    T.rows[1][1] = 1.0f / tf;
    T.rows[2][2] = f / (f - n);
    T.rows[2][3] = -(f * n) / (f - n);
    T.rows[3][2] = 1.0f;

    m4 S = m4::create_identity();
    S.rows[0][0] = 0.5f;
    S.rows[1][1] = 0.5f;
    S.rows[2][2] = 1.0f;

    S.rows[0][3] = 0.5f;
    S.rows[1][3] = 0.5f;
    S.rows[3][3] = 1.0f;

    m4 R = S * T;

    return R;
}

m4 m4::
create_clip_to_device(int width, int height)
{
    m4 T = m4::create_identity();
    T.rows[0][0] = (width - 1);
    T.rows[1][1] = -(height - 1);
    T.rows[1][3] = height - 1;
    return T;
}

v4
operator*(const m4& lhs, const v4& rhs)
{

    v4 result;
    result[0] = (lhs.rows[0][0] * rhs[0]) + (lhs.rows[0][1] * rhs[1]) + (lhs.rows[0][2] * rhs[2]) + (lhs.rows[0][3] * rhs[3]);
    result[1] = (lhs.rows[1][0] * rhs[0]) + (lhs.rows[1][1] * rhs[1]) + (lhs.rows[1][2] * rhs[2]) + (lhs.rows[1][3] * rhs[3]);
    result[2] = (lhs.rows[2][0] * rhs[0]) + (lhs.rows[2][1] * rhs[1]) + (lhs.rows[2][2] * rhs[2]) + (lhs.rows[2][3] * rhs[3]);
    result[3] = (lhs.rows[3][0] * rhs[0]) + (lhs.rows[3][1] * rhs[1]) + (lhs.rows[3][2] * rhs[2]) + (lhs.rows[3][3] * rhs[3]);
    return result;

}

// --- Output Stream Overloads -------------------------------------------------

#ifdef LINEAR_DEFINE_OSTREAM
#include <iostream>
std::ostream&
operator<<(std::ostream& out, const v3& v)
{
    out << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
    return out;
}

std::ostream&
operator<<(std::ostream& out, const v4& v)
{
    out << "{ " << v.x << ", " << v.y << ", "
        << v.z << ", " << v.w << " }";
    return out;
}

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
            std::cout << ", " << std::endl << "  ";

    };

    std::cout << " }";

    return out;
}
#endif
