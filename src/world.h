#ifndef WORLD_H
#define WORLD_H

#include "input.h"
#include "player.h"
#include "renderer.h"
#include "chunkmap.h"
#include "worldgen.h"
#include "util/util.h"

#define RENDER_DIST 8
#define BUFFER_DIST 3
#define GRID_SIZE ((RENDER_DIST + BUFFER_DIST) * 2 + 1)

#define CHUNK_QUEUE_SIZE ((RENDER_DIST * 2) + 1) * ((RENDER_DIST * 2) + 1)

struct ChunkQueue
{
    ivec2s offsets[CHUNK_QUEUE_SIZE];
    uint32_t count;
};

struct World
{
    struct Player player;

    struct ChunkMap* map;

    struct Chunk** chunks;  // list of loaded chunks for iteration
    size_t chunk_count;

    struct ChunkQueue chunkqueue;
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

void world_init(struct World* world);
bool world_ray_cast(struct World* world, vec3s origin, vec3s direction, float max_distance, ivec3s* hit, ivec3s* normal);
struct Chunk* world_get_chunk(struct World* world, ivec2s offset);
uint8_t world_get_block(struct World* world, ivec3s pos);
void world_set_block(struct World* world, ivec3s pos, uint8_t block_id);
void world_load_chunk(struct World* world, ivec2s offset);
void world_unload_chunk(struct World* world, size_t index);
void world_update(struct World* world, struct Input* input);
void world_render(struct World* world, struct Renderer* renderer);
void world_destroy(struct World* world);

static inline void chunkqueue_init(struct ChunkQueue* q)
{
    q->count = 0;
}

static inline bool chunkqueue_contains(struct ChunkQueue* q, ivec2s offset)
{
    for (size_t i = 0; i < q->count; i++) {
        if (q->offsets[i].x == offset.x && q->offsets[i].y == offset.y)
            return true;
    }
    return false;
}

static inline void chunkqueue_push(struct ChunkQueue* q, ivec2s offset)
{
    if (q->count >= CHUNK_QUEUE_SIZE)
        return;

    if (chunkqueue_contains(q, offset))
        return;

    q->offsets[q->count++] = offset;
}

static inline bool chunkqueue_pop(struct ChunkQueue* q, ivec2s* out_offset)
{
    if (q->count == 0)
        return false;

    *out_offset = q->offsets[--q->count];

    return true;
}

static inline void chunkqueue_sort(struct ChunkQueue* q, int(*compare_fn)(const void*, const void*))
{
    if (q->count <= 1)
        return;
    
    qsort(q->offsets, q->count, sizeof(ivec2s), compare_fn);
}

#endif