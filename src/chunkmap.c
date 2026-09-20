#include "chunkmap.h"

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

    uint32_t distance = 0;

    while (true)
    {
        struct ChunkEntry* entry = &map->data[index];

        if (!entry->occupied)
            return NULL;

        if (entry->distance < distance)
            return NULL;

        if (entry->key == key)
            return entry->chunk;

        index = (index + 1) & (map->capacity - 1);
        distance++;
    }
}

void chunkmap_put(struct ChunkMap* map, int32_t x, int32_t z, struct Chunk* chunk)
{
    if (map->capacity < map->count * 2)
    {
        chunkmap_resize(map);
    }

    uint64_t key = chunk_key(x, z);
    uint64_t hash = chunk_hash(key);
    size_t index = hash & (map->capacity - 1);

    struct ChunkEntry entry = {
        .key = key,
        .chunk = chunk,
        .distance = 0,
        .occupied = true
    };

    while (true)
    {
        struct ChunkEntry* current = &map->data[index];

        if (!current->occupied)
        {
            *current = entry;
            map->count++;
            return;
        }

        if (current->key == key)
        {
            current->chunk = chunk; // update existing
            return;
        }

        if (current->distance < entry.distance)
        {
            struct ChunkEntry temp = *current; // swap with the displaced entry
            *current = entry;
            entry = temp;
        }

        index = (index + 1) & (map->capacity - 1);
        entry.distance++;
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

    // Shift following entries backward

    size_t next = (index + 1) & (map->capacity - 1);

    while (map->data[next].occupied && map->data[next].distance > 0)
    {
        map->data[index] = map->data[next];
        map->data[index].distance--;

        index = next;
        next = (next + 1) & (map->capacity - 1);
    }

    map->data[index].occupied = false;
    map->count--;
}

void chunkmap_resize(struct ChunkMap* map)
{
    size_t new_capacity = map->capacity * 2;
    struct ChunkEntry* new_data = calloc(new_capacity, sizeof(struct ChunkEntry));

    for (size_t i = 0; i < map->capacity; i++)
    {
        if (!map->data[i].occupied) continue;

        struct ChunkEntry entry = map->data[i];
        size_t index = chunk_hash(entry.key) & (new_capacity - 1);
        entry.distance = 0;

        while (true)
        {
            struct ChunkEntry* current = &new_data[index];

            if (!current->occupied)
            {
                *current = entry;
                break;
            }

            if (current->distance < entry.distance)
            {
                struct ChunkEntry temp = *current;
                *current = entry;
                entry = temp;
            }

            index = (index + 1) & (new_capacity - 1);
            entry.distance++;
        }
    }

    free(map->data);
    map->data = new_data;
    map->capacity = new_capacity;
}

void chunkmap_destroy(struct ChunkMap* map)
{
    free(map->data);
    free(map);
}