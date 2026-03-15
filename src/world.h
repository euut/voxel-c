#ifndef WORLD_H
#define WORLD_H

#include "input.h"
#include "camera.h"
#include "renderer.h"
#include "chunkmap.h"
#include "util/math.h"

#define RENDER_DISTANCE 8

struct World
{
    struct Camera camera;

    struct ChunkMap* chunkmap;

    struct Chunk** chunks;  // dynamic list of loaded chunks
    size_t chunk_count;
    size_t chunk_capacity;

    enum BlockId selected_block;
};

// Convert world position to chunk offset
static inline ivec2s world_to_chunk_offset(ivec3s world_pos)
{
    ivec2s offset;

    offset.x = world_pos.x >> 4;
    offset.y = world_pos.z >> 4;

    return offset;
}

// Convert world position to local block position within chunk
static inline ivec3s world_to_local_pos(ivec3s world_pos)
{
    ivec3s local;

    local.x = world_pos.x & (CHUNK_WIDTH - 1);
    local.y = world_pos.y;
    local.z = world_pos.z & (CHUNK_WIDTH - 1);

    return local;
}

void world_init(struct World* world);
struct Chunk* world_get_chunk(struct World* world, ivec2s offset);
uint8_t world_get_block(struct World* world, ivec3s pos);
void world_set_block(struct World* world, ivec3s pos, uint8_t block_id);
void world_load_chunk(struct World* world, ivec2s offset);
void world_unload_chunk(struct World* world, size_t index);
void world_update(struct World* world, struct Input* input);
void world_render(struct World* world, struct Renderer* renderer);
void world_destroy(struct World* world);

#endif