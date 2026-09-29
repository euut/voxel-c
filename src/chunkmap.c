#include "chunkmap.h"
#include <stdlib.h>
#include <stdbool.h>

static inline uint64_t chunk_key(int32_t x, int32_t z)
{
    return ((uint64_t)(uint32_t)x << 32) | (uint32_t)z;
}

static inline uint64_t chunk_hash(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;
    return k;
}

struct ChunkMap* chunkmap_create(size_t capacity)
{
    struct ChunkMap* map = malloc(sizeof(struct ChunkMap));

    map->capacity = capacity;
    map->count = 0;
    map->data = calloc(capacity, sizeof(struct ChunkEntry));

    return map;
}

struct Chunk* chunkmap_get(struct ChunkMap* map, int32_t x, int32_t z)
{
    uint64_t key = chunk_key(x, z);
    uint64_t hash = chunk_hash(key);
    size_t index = hash & (map->capacity - 1);

    while (true)
    {
        struct ChunkEntry* entry = &map->data[index];

        if (!entry->occupied)
            return NULL;

        if (entry->key == key)
            return entry->chunk;

        index = (index + 1) & (map->capacity - 1);
    }
}

void chunkmap_put(struct ChunkMap* map, int32_t x, int32_t z, struct Chunk* chunk)
{
    uint64_t key = chunk_key(x, z);
    uint64_t hash = chunk_hash(key);
    size_t index = hash & (map->capacity - 1);

    while (true)
    {
        struct ChunkEntry* current = &map->data[index];

        if (!current->occupied)
        {
            current->key = key;
            current->chunk = chunk;
            current->occupied = true;
            map->count++;
            return;
        }

        // update if the key already exists
        if (current->key == key)
        {
            current->chunk = chunk;
            return;
        }

        index = (index + 1) & (map->capacity - 1);
    }
}

void chunkmap_remove(struct ChunkMap* map, int32_t x, int32_t z)
{
    uint64_t key = chunk_key(x, z);
    uint64_t hash = chunk_hash(key);
    size_t index = hash & (map->capacity - 1);

    while (true)
    {
        struct ChunkEntry* entry = &map->data[index];

        if (!entry->occupied)
            return;

        if (entry->key == key)
            break;

        index = (index + 1) & (map->capacity - 1);
    }

    // Shift entries backward
    size_t next = (index + 1) & (map->capacity - 1);

    while (map->data[next].occupied)
    {
        uint64_t next_hash = chunk_hash(map->data[next].key);
        size_t ideal_index = next_hash & (map->capacity - 1);

        if ((next > index && (ideal_index <= index || ideal_index > next)) ||
            (next < index && (ideal_index <= index && ideal_index > next)))
        {
            map->data[index] = map->data[next];
            index = next;
        }
        
        next = (next + 1) & (map->capacity - 1);
    }

    map->data[index].occupied = false;
    map->count--;
}

void chunkmap_destroy(struct ChunkMap* map)
{
    free(map->data);
    free(map);
}