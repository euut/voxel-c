#include "world.h"

void world_init(struct World* world, int seed)
{
    memset(world, 0, sizeof(struct World));
    
    world->chunkmap = chunkmap_create(128);
    world->render_distance = 8;

    player_init(&world->player, (vec3s) {{ 0, 64, 0 }}, world);
    worldgen_init(seed);
}

bool world_ray_cast(struct World* world, vec3s origin, vec3s direction, float max_distance, ivec3s* hit, ivec3s* normal)
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

    tmax.x = (direction.x != 0) ? ((pos.x + (step.x > 0 ? 1.0f : 0.0f)) - origin.x) / direction.x : FLT_MAX;
    tmax.y = (direction.y != 0) ? ((pos.y + (step.y > 0 ? 1.0f : 0.0f)) - origin.y) / direction.y : FLT_MAX;
    tmax.z = (direction.z != 0) ? ((pos.z + (step.z > 0 ? 1.0f : 0.0f)) - origin.z) / direction.z : FLT_MAX;

    float distance = 0.0f;

    ivec3s n = GLMS_IVEC3_ZERO; // which face the ray is pointing
    
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

struct Chunk* world_get_chunk(struct World* world, ivec2s offset)
{
    return chunkmap_get(world->chunkmap, offset.x, offset.y);
}

bool world_chunk_exists(struct World* world, ivec2s offset)
{
    return world_get_chunk(world, offset) != NULL;
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
    if (world_chunk_exists(world, offset)) return;

    struct Chunk* chunk = malloc(sizeof(struct Chunk));

    chunk_init(chunk, world, offset);

    worldgen_generate_terrain(chunk);

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

void world_update(struct World* world, struct Input* input)
{
    ivec2s player_chunk_offset = world_to_chunk_offset(vec3s_to_ivec3s(world->player.position));

    for (int dx = -world->render_distance; dx <= world->render_distance; dx++)
    {
        for (int dz = -world->render_distance; dz <= world->render_distance; dz++)
        {
            ivec2s offset = glms_ivec2_add(player_chunk_offset, (ivec2s){{dx, dz}});

            world_load_chunk(world, offset);
        }
    }

    // unload chunks away from the player
    size_t i = 0;
    while (i < world->chunk_count)
    {
        struct Chunk* chunk = world->chunks[i];

        int dx = abs(chunk->offset.x - player_chunk_offset.x);
        int dz = abs(chunk->offset.y - player_chunk_offset.y);

        if (dx > world->render_distance + 2 || dz > world->render_distance + 2)
        {
            world_unload_chunk(world, i);
            continue;
        }

        i++;
    }

    player_update(&world->player, input);
}

void world_render(struct World* world, struct Renderer* renderer)
{
    renderer->camera = &world->player.camera;

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
