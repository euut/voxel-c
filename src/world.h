#ifndef WORLD_H
#define WORLD_H

#include "input.h"
#include "player.h"
#include "renderer.h"
#include "chunkmap.h"
#include "worldgen.h"
#include "util/math.h"

struct World
{
    int render_distance;

    struct Player player;

    struct ChunkMap* chunkmap;

    struct Chunk** chunks;  // dynamic list of loaded chunks
    size_t chunk_count;
    size_t chunk_capacity;
};

struct Ray
{
    bool hit;
    ivec3s target_block;
    ivec3s normal;
};

// Convert world position to chunk offset
static inline ivec2s world_to_chunk_offset(ivec3s world_pos)
{
    return (ivec2s) {
        world_pos.x >> 4,
        world_pos.z >> 4
    };
}

// Convert world position to local block position within chunk
static inline ivec3s world_to_local_pos(ivec3s world_pos)
{
    return (ivec3s) {
        world_pos.x & (CHUNK_WIDTH - 1),
        world_pos.y,
        world_pos.z & (CHUNK_WIDTH - 1)
    };
}

void world_init(struct World* world, int seed);
bool world_ray_cast(struct World* world, vec3s origin, vec3s direction, float max_distance, ivec3s* hit, ivec3s* normal);
struct Chunk* world_get_chunk(struct World* world, ivec2s offset);
uint8_t world_get_block(struct World* world, ivec3s pos);
void world_set_block(struct World* world, ivec3s pos, uint8_t block_id);
void world_load_chunk(struct World* world, ivec2s offset);
void world_unload_chunk(struct World* world, size_t index);
void world_update(struct World* world, struct Input* input);
void world_render(struct World* world, struct Renderer* renderer);
void world_destroy(struct World* world);

#endif