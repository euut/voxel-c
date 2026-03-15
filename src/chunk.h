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
#define CHUNK_HEIGHT 16
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

void chunk_init(struct Chunk* chunk, struct World* world, ivec2s offset);
uint8_t chunk_get_block(struct Chunk* chunk, ivec3s pos);
void chunk_set_block(struct Chunk* chunk, ivec3s pos, uint8_t block_id);
void chunk_build_mesh(struct Chunk* chunk, struct Renderer* renderer);
void chunk_render(struct Chunk* chunk, struct Renderer* renderer);
void chunk_destroy(struct Chunk* chunk);

#endif