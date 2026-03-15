#ifndef MATH_H
#define MATH_H

#include <cglm/cglm.h>
#include <cglm/struct.h>

#define radians(deg) glm_rad(deg)
#define degrees(rad) glm_deg(rad)

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