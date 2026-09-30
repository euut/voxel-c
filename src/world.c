#include "world.h"

static ivec3s camera_pos;

static int compare_chunks_back_to_front(const void *a, const void *b)
{
    const struct Chunk* aa = *(const struct Chunk**)a;
    const struct Chunk* bb = *(const struct Chunk**)b;

    int da = glms_ivec3_norm2(glms_ivec3_sub(camera_pos, chunk_local_to_world(aa->offset, GLMS_IVEC3_ZERO)));
    int db = glms_ivec3_norm2(glms_ivec3_sub(camera_pos, chunk_local_to_world(bb->offset, GLMS_IVEC3_ZERO)));

    return (db > da) - (db < da);;
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
        if (block_is_solid(world_get_block(world, pos)))
        {
            *hit = pos;
            *normal = n;
            return true;
        }

        if (tmax.x < tmax.y && tmax.x < tmax.z)
        {
            pos.x += step.x;
            distance = tmax.x;
            tmax.x += tdelta.x;
            n = (ivec3s){{-step.x, 0, 0}};
        }
        else if (tmax.y < tmax.z)
        {
            pos.y += step.y;
            distance = tmax.y;
            tmax.y += tdelta.y;
            n = (ivec3s){{0, -step.y, 0}};
        }
        else
        {
            pos.z += step.z;
            distance = tmax.z;
            tmax.z += tdelta.z;
            n = (ivec3s){{0, 0, -step.z}};
        }
    }
    
    return false;
}

void world_init(struct World* world)
{
    memset(world, 0, sizeof(struct World));
    
    world->map = chunkmap_create(1024);
    
    world->chunks = malloc(GRID_SIZE * GRID_SIZE * sizeof(struct Chunk*));
    world->chunk_capacity = GRID_SIZE * GRID_SIZE;

    player_init(&world->player, (vec3s) {{ 0, 64, 0 }}, world);

    worldgen_init(12345);
}

struct Chunk* world_get_chunk(struct World* world, ivec2s offset)
{
    return chunkmap_get(world->map, offset.x, offset.y);
}

bool world_chunk_exists(struct World* world, ivec2s offset)
{
    return chunkmap_get(world->map, offset.x, offset.y) != NULL;
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
    if (world_chunk_exists(world, offset))
        return;

    struct Chunk* chunk = malloc(sizeof(struct Chunk));

    chunk_init(chunk, world, offset);

    worldgen_generate_terrain(chunk);

    chunkmap_put(world->map, offset.x, offset.y, chunk);
    
    world->chunks[world->chunk_count++] = chunk;
}

void world_unload_chunk(struct World* world, size_t index)
{
    struct Chunk* chunk = world->chunks[index];

    chunkmap_remove(world->map, chunk->offset.x, chunk->offset.y);

    world->chunks[index] = world->chunks[--world->chunk_count];

    chunk_destroy(chunk); // free chunk memory
    free(chunk);
}

void world_update(struct World* world, struct Input* input)
{
    ivec2s player_chunk_offset = world_to_chunk_offset(vec3s_to_ivec3s(world->player.position));

    for (int dx = -RENDER_DIST; dx <= RENDER_DIST; dx++)
    {
        for (int dz = -RENDER_DIST; dz <= RENDER_DIST; dz++)
        {
            world_load_chunk(world, glms_ivec2_add(player_chunk_offset, (ivec2s){{dx, dz}}));
        }
    }

    // unload chunks away from the player
    size_t i = 0;
    while (i < world->chunk_count)
    {
        struct Chunk* chunk = world->chunks[i];

        int dx = abs(chunk->offset.x - player_chunk_offset.x);
        int dz = abs(chunk->offset.y - player_chunk_offset.y);

        if (dx > RENDER_DIST + BUFFER_DIST || dz > RENDER_DIST + BUFFER_DIST)
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

    // render opaque meshes first
    for (size_t i = 0; i < world->chunk_count; i++)
    {
        chunk_render_opaque(world->chunks[i], renderer);
    }

    camera_pos = vec3s_to_ivec3s(world->player.camera.position);

    // sort chunks back to front based on distance from camera
    qsort(world->chunks, world->chunk_count, sizeof(struct Chunk*), compare_chunks_back_to_front);

    // render transparent meshes after sorting
    for (size_t i = 0; i < world->chunk_count; i++)
    {
        chunk_render_transparent(world->chunks[i], renderer);
    }
}

void world_destroy(struct World* world)
{
    chunkmap_destroy(world->map);

    for (size_t i = 0; i < world->chunk_count; i++)
    {
        chunk_destroy(world->chunks[i]);
        free(world->chunks[i]);
    }

    free(world->chunks);
}
