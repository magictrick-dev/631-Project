#ifndef J5_CORE_PRIMITIVES_H
#define J5_CORE_PRIMITIVES_H
#include <cassert>
#include <cstdint>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef float       f32;
typedef double      f64;

#define PI32        3.14159265358979f
#define DEGREES_TO_RADIANS(param) ((param) * (PI32 / 180.0f))

#endif
