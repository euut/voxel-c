#ifndef UTIL_H
#define UTIL_H

#include <math.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

#define radians(deg) glm_rad(deg)
#define degrees(rad) glm_deg(rad)

static const ivec3s FACE_NORMALS[6] =
{
    {{  0,  0, -1 }}, // +Z (front)
    {{  0,  0,  1 }}, // -Z (back)
    {{  1,  0,  0 }}, // +X (right)
    {{ -1,  0,  0 }}, // -X (left)
    {{  0,  1,  0 }}, // +Y (top)
    {{  0, -1,  0 }} // -Y (bottom)
};

static inline vec2s ivec2s_to_vec2s(ivec2s vec)
{
    return (vec2s){ vec.x, vec.y };
}

static inline ivec2s vec2s_to_ivec2s(vec2s vec)
{
    return (ivec2s){ vec.x, vec.y };
}

static inline vec3s ivec3s_to_vec3s(ivec3s vec)
{
    return (vec3s){ vec.x, vec.y, vec.z };
}

static inline ivec3s vec3s_to_ivec3s(vec3s vec)
{
    return (ivec3s){ vec.x, vec.y, vec.z };
}

#endif