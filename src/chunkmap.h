#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include "chunk.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

enum ChunkEntryState
{
    ENTRY_EMPTY = 0,
    ENTRY_OCCUPIED
};

struct ChunkEntry
{
    uint64_t key;
    uint32_t distance;
    struct Chunk* chunk;
    enum ChunkEntryState state;
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
void chunkmap_resize(struct ChunkMap* map);
void chunkmap_destroy(struct ChunkMap* map);

#endif