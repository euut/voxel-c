#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include "chunk.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct ChunkEntry
{
    uint64_t key;
    struct Chunk* chunk;
    bool occupied;
};

struct ChunkMap
{
    struct ChunkEntry* data;
    size_t capacity;
    size_t count;
};

struct ChunkMap* chunkmap_create(size_t capacity);
struct Chunk* chunkmap_get(struct ChunkMap* map, int32_t x, int32_t z);
void chunkmap_put(struct ChunkMap* map, int32_t x, int32_t z, struct Chunk* chunk);
void chunkmap_remove(struct ChunkMap* map, int32_t x, int32_t z);
void chunkmap_destroy(struct ChunkMap* map);

#endif