#include "world.h"

static void worldgen_generate(struct Chunk* chunk)
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                uint8_t block = BLOCK_AIR;

                if (y < 4)
                    block = BLOCK_GRASS;
                if (y < 3)
                    block = BLOCK_DIRT;

                chunk_set_block(chunk, (ivec3s){{x, y, z}}, block);
            }
        }
    }
}

static bool world_ray_cast(struct World* world, vec3s origin, vec3s direction, float max_distance, ivec3s* hit, ivec3s* normal)
{
    ivec3s pos, step = GLMS_IVEC3_ZERO;
    
    pos.x = (int)floorf(origin.x);
    pos.y = (int)floorf(origin.y);
    pos.z = (int)floorf(origin.z);

    step.x = (direction.x > 0) - (direction.x < 0);
    step.y = (direction.y > 0) - (direction.y < 0);
    step.z = (direction.z > 0) - (direction.z < 0);
    
    vec3s tmax, tdelta = GLMS_VEC3_ZERO;
    
    tdelta.x = (direction.x != 0) ? fabsf(1.0f / direction.x) : FLT_MAX;
    tdelta.y = (direction.y != 0) ? fabsf(1.0f / direction.y) : FLT_MAX;
    tdelta.z = (direction.z != 0) ? fabsf(1.0f / direction.z) : FLT_MAX;

    tmax.x = ((pos.x + (step.x > 0 ? 1.0f : 0.0f)) - origin.x) / direction.x;
    tmax.y = ((pos.y + (step.y > 0 ? 1.0f : 0.0f)) - origin.y) / direction.y;
    tmax.z = ((pos.z + (step.z > 0 ? 1.0f : 0.0f)) - origin.z) / direction.z;

    float distance = 0.0f;

    ivec3s n = GLMS_IVEC3_ZERO;; // which face the ray is pointing
    
    while (distance <= max_distance)
    {
        if (world_get_block(world, pos) != BLOCK_AIR)
        {
            *hit = pos;
            *normal = n;
            return true;
        }

        if (tmax.x < tmax.y)
        {
            if (tmax.x < tmax.z)
            {
                pos.x += step.x;
                distance = tmax.x;
                tmax.x += tdelta.x;
                n = (ivec3s) {{-step.x, 0, 0}};
            }
            else
            {
                pos.z += step.z;
                distance = tmax.z;
                tmax.z += tdelta.z;
                n = (ivec3s) {{0, 0, -step.z}};
            }
        }
        else
        {
            if (tmax.y < tmax.z)
            {
                pos.y += step.y;
                distance = tmax.y;
                tmax.y += tdelta.y;
                n = (ivec3s) {{0, -step.y, 0}};
            }
            else
            {
                pos.z += step.z;
                distance = tmax.z;
                tmax.z += tdelta.z;
                n = (ivec3s) {{0, 0, -step.z}};
            }
        }
    }
    
    return false;
}

void world_init(struct World* world)
{
    memset(world, 0, sizeof(struct World));
    
    world->chunkmap = chunkmap_create(128);
    world->selected_block = BLOCK_DIRT;
    camera_init(&world->camera, (vec3s){{0,7,0}}, radians(75.0f));
}

struct Chunk* world_get_chunk(struct World* world, ivec2s offset)
{
    return chunkmap_get(world->chunkmap, offset.x, offset.y);
}

uint8_t world_get_block(struct World* world, ivec3s pos)
{
    struct Chunk* chunk = world_get_chunk(world, world_to_chunk_offset(pos));

    if (!chunk)
    {
        return BLOCK_AIR;
    }

    return chunk_get_block(chunk, world_to_local_pos(pos));
}

void world_set_block(struct World* world, ivec3s pos, uint8_t block_id)
{
    ivec2s offset = world_to_chunk_offset(pos);

    struct Chunk* chunk = world_get_chunk(world, offset);

    if (!chunk)
    {
        world_load_chunk(world, offset);
        chunk = world_get_chunk(world, offset);
    }

    chunk_set_block(chunk, world_to_local_pos(pos), block_id);
}

void world_load_chunk(struct World* world, ivec2s offset)
{
    // Skip chunks that already exist
    if (world_get_chunk(world, offset) != NULL) return;

    struct Chunk* chunk = malloc(sizeof(struct Chunk));
    chunk_init(chunk, world, offset);
    worldgen_generate(chunk);

    chunkmap_put(world->chunkmap, offset.x, offset.y, chunk);

    // Add new chunk to dynamic list
    if (world->chunk_count + 1 > world->chunk_capacity)
    {
        world->chunk_capacity = world->chunk_capacity ? world->chunk_capacity * 2 : 64;
        world->chunks = realloc(world->chunks, world->chunk_capacity * sizeof(struct Chunk*));
    }
    
    world->chunks[world->chunk_count++] = chunk;
}

void world_unload_chunk(struct World* world, size_t index)
{
    struct Chunk* chunk = world->chunks[index];

    chunkmap_remove(world->chunkmap, chunk->offset.x, chunk->offset.y);

    world->chunks[index] = world->chunks[--world->chunk_count];

    chunk_destroy(chunk); // free chunk memory
    free(chunk);
}

void world_move_camera(struct World* world, struct Input* input)
{
    float speed = input_key_down(input, GLFW_KEY_LEFT_CONTROL) ? 0.4f : 0.2f;

    camera_rotate(&world->camera, input);

    // printf("%f, %f\n", input->mouse_delta.x, input->mouse_delta.y);

    vec3s offset = GLMS_VEC3_ZERO;
    vec3s forward = glms_vec3_normalize((vec3s){{world->camera.front.x, 0.0f, world->camera.front.z}});
    vec3s right = glms_vec3_normalize(glms_vec3_cross(forward, (vec3s) {{0.0f, 1.0f, 0.0f}}));

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
        offset = glms_vec3_add(offset, (vec3s) {{0.0f, 1.0f, 0.0f}});
    }

    if (input_key_down(input, GLFW_KEY_LEFT_SHIFT))
    {
        offset = glms_vec3_sub(offset, (vec3s) {{0.0f, 1.0f, 0.0f}});
    }

    if (!glms_vec3_eq(offset, 0.0f))
    {
        offset = glms_vec3_normalize(offset);
        offset = glms_vec3_scale(offset, speed);
    }
    
    camera_move(&world->camera, offset);
}

void world_update(struct World* world, struct Input* input)
{
    world_move_camera(world, input);

    ivec2s player_chunk_offset = world_to_chunk_offset(vec3s_to_ivec3s(world->camera.position));

    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++)
    {
        for (int dz = -RENDER_DISTANCE; dz <= RENDER_DISTANCE; dz++)
        {
            ivec2s offset = glms_ivec2_add(player_chunk_offset, (ivec2s){{dx, dz}});

            if (abs(glms_ivec2_distance(offset, player_chunk_offset)) <= RENDER_DISTANCE)
            {
                world_load_chunk(world, offset);
            }
        }
    }

    // unload chunks away from the player
    size_t i = 0;
    while (i < world->chunk_count)
    {
        struct Chunk* chunk = world->chunks[i];

        if (abs(glms_ivec2_distance(chunk->offset, player_chunk_offset)) > RENDER_DISTANCE)
        {
            world_unload_chunk(world, i);
            // do NOT increment i here, because world_unload_chunk swapped in a new chunk at index i
            continue;
        }

        i++;
    }

    for (enum BlockId i = 1; i < BLOCK_TOTAL; i++)
    {
        if (input_key_down(input, GLFW_KEY_0 + i))
        {
            world->selected_block = i;
        }
    }

    ivec3s hit, face = GLMS_IVEC3_ZERO;
    
    bool target_hit = world_ray_cast(world, world->camera.position, world->camera.front, 8.0f, &hit, &face);

    if (target_hit)
    {
        // break block
        if (input_mouse_pressed(input, GLFW_MOUSE_BUTTON_LEFT))
        {
            world_set_block(world, hit, BLOCK_AIR);
        }

        // place block
        if (input_mouse_pressed(input, GLFW_MOUSE_BUTTON_RIGHT))
        {
            world_set_block(world, glms_ivec3_add(hit, face), world->selected_block);
        }
    }

    camera_update(&world->camera);
}

void world_render(struct World* world, struct Renderer* renderer)
{
    renderer->camera = &world->camera;

    for (size_t i = 0; i < world->chunk_count; i++)
    {
        chunk_render(world->chunks[i], renderer);
    }
}

void world_destroy(struct World* world)
{
    chunkmap_destroy(world->chunkmap);

    for (size_t i = 0; i < world->chunk_count; i++)
    {
        chunk_destroy(world->chunks[i]);
        free(world->chunks[i]);
    }

    free(world->chunks);
}
