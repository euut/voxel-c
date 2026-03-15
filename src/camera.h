#ifndef CAMERA_H
#define CAMERA_H

#include "input.h"
#include "util/math.h"
#include <string.h>

struct Camera
{
	vec3s position;
    vec3s front, right, up;
    mat4s projection_matrix;
    mat4s view_matrix;

    float yaw, pitch;
    float fov, aspect, near, far;
};

void camera_init(struct Camera* camera, vec3s position, float fov);
void camera_update(struct Camera* camera);
void camera_rotate(struct Camera* camera, struct Input* input);
void camera_move(struct Camera* camera, vec3s offset);

#endif