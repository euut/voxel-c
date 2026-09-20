#include "player.h"
#include "world.h"

void player_init(struct Player* player, vec3s position, struct World* world)
{
    camera_init(&player->camera, position, radians(75.0f));

    player->position = position;
    player->world = world;
    player->selected_block = BLOCK_DIRT;
}

void player_update(struct Player* player, struct Input* input)
{
    player->speed = input_key_down(input, GLFW_KEY_LEFT_CONTROL) ? 0.4f : 0.2f;

    camera_rotate(&player->camera, input);

    // printf("%f, %f\n", input->mouse_delta.x, input->mouse_delta.y);

    vec3s offset = GLMS_VEC3_ZERO;
    vec3s forward = glms_vec3_normalize((vec3s){{ player->camera.front.x, 0.0f, player->camera.front.z }});
    vec3s right = glms_vec3_normalize(glms_vec3_cross(forward, (vec3s) {{ 0.0f, 1.0f, 0.0f }}));

    if (input_key_down(input, GLFW_KEY_W))
    {
        offset = glms_vec3_add(offset, forward);
    }

    if (input_key_down(input, GLFW_KEY_S))
    {
        offset = glms_vec3_sub(offset, forward);
    }

    if (input_key_down(input, GLFW_KEY_A))
    {
        offset = glms_vec3_sub(offset, right);
    }

    if (input_key_down(input, GLFW_KEY_D))
    {
        offset = glms_vec3_add(offset, right);
    }

    if (input_key_down(input, GLFW_KEY_SPACE))
    {
        offset = glms_vec3_add(offset, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    }

    if (input_key_down(input, GLFW_KEY_LEFT_SHIFT))
    {
        offset = glms_vec3_sub(offset, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    }

    if (!glms_vec3_eq(offset, 0.0f))
    {
        offset = glms_vec3_normalize(offset);
        offset = glms_vec3_scale(offset, player->speed);
    }
    
    camera_move(&player->camera, offset);

    player->position = player->camera.position;

    camera_update(&player->camera);

    // select block (may add gui later for block selection)
    for (enum BlockId i = 1; i < BLOCK_TOTAL; i++)
    {
        if (input_key_down(input, GLFW_KEY_0 + i))
        {
            player->selected_block = i;
        }
    }

    ivec3s hit, face = GLMS_IVEC3_ZERO;
    
    // cast a ray from camera position to find the block player is looking at, true if a block was hit 
    bool target_hit = world_ray_cast(player->world, player->camera.position, player->camera.front, 8.0f, &hit, &face);

    if (target_hit)
    {
        // break block
        if (input_mouse_pressed(input, GLFW_MOUSE_BUTTON_LEFT))
        {
            world_set_block(player->world, hit, BLOCK_AIR);
        }

        // place block
        if (input_mouse_pressed(input, GLFW_MOUSE_BUTTON_RIGHT))
        {
            world_set_block(player->world, glms_ivec3_add(hit, face), player->selected_block);
        }
    }
}
