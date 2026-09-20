#ifndef CHUNK_H
#define CHUNK_H

#include "mesh.h"
#include "block.h"
#include "renderer.h"
#include "util/math.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 64
#define CHUNK_BLOCK_TOTAL (CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT)

struct World;

struct Chunk
{
    ivec2s offset; // chunk coords (cx, cz)
    
    uint8_t* blocks;
    
    struct Mesh mesh;

    struct World* world;

    bool dirty; // true if the chunk mesh needs rebuilding
};

static inline int chunk_block_index(ivec3s pos)
{
    return pos.x + pos.z * CHUNK_WIDTH + pos.y * CHUNK_WIDTH * CHUNK_WIDTH;
}

// Convert local block coords to world position
static inline ivec3s chunk_local_to_world(ivec2s chunk_offset, ivec3s local_pos)
{
    return (ivec3s) {
        .x = chunk_offset.x * CHUNK_WIDTH + local_pos.x,
        .y = local_pos.y,
        .z = chunk_offset.y * CHUNK_WIDTH + local_pos.z
    };
}

void chunk_init(struct Chunk* chunk, struct World* world, ivec2s offset);
uint8_t chunk_get_block(struct Chunk* chunk, ivec3s pos);
void chunk_set_block(struct Chunk* chunk, ivec3s pos, uint8_t block_id);
void chunk_build_mesh(struct Chunk* chunk, struct TextureAtlas* atlas);
void chunk_render(struct Chunk* chunk, struct Renderer* renderer);
void chunk_destroy(struct Chunk* chunk);

#endif