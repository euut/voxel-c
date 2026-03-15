#include "camera.h"

void camera_init(struct Camera* camera, vec3s position, float fov)
{
    memset(camera, 0, sizeof(struct Camera));

    camera->position = position;

    camera->fov = fov;
    camera->aspect = 1920.0f / 1080.0f;
    camera->near = 0.01f;
    camera->far = 1000.0f;

    // Set an initial direction
    camera_update(camera);
}

void camera_update(struct Camera* camera)
{   
    camera->front = (vec3s) {{
        cosf(radians(camera->yaw)) * cosf(radians(camera->pitch)),
        sinf(radians(camera->pitch)),
        sinf(radians(camera->yaw)) * cosf(radians(camera->pitch))
    }};

    camera->front = glms_vec3_normalize(camera->front);
    camera->right = glms_vec3_normalize(glms_vec3_cross(camera->front, (vec3s){{0,1,0}}));
    camera->up = glms_vec3_normalize(glms_vec3_cross(camera->right, camera->front));

    camera->view_matrix = glms_lookat(camera->position, glms_vec3_add(camera->position, camera->front), camera->up);
    camera->projection_matrix = glms_perspective(camera->fov, camera->aspect, camera->near, camera->far);
}

void camera_rotate(struct Camera* camera, struct Input* input)
{
    float sensitivity = 0.4f;

    camera->yaw += input->mouse_delta.x * sensitivity;
    camera->pitch -= input->mouse_delta.y * sensitivity;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;
    
    camera->yaw = fmodf(camera->yaw + 180.0f, 360.0f) - 180.0f;
}

void camera_move(struct Camera* camera, vec3s offset)
{
    camera->position = glms_vec3_add(camera->position, offset);
}
