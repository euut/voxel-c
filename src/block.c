#include "block.h"

static struct Block blocks[BLOCK_TOTAL] =
{
    [BLOCK_AIR] =
    {
        .solid = false,
        .transparent = true
    },

    [BLOCK_DIRT] =
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 2, 2, 2, 2, 2, 2 }
    },

    [BLOCK_GRASS] =
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 3, 3, 3, 3, 0, 2 }
    },

    [BLOCK_STONE] =
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 1, 1, 1, 1, 1, 1 }
    },

    [BLOCK_SAND] =
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 18, 18, 18, 18, 18, 18 }
    },

    [BLOCK_WATER] =
    {
        .solid = false,
        .transparent = true,
        .tile_index = { 205, 205, 205, 205, 205, 205 }
    },

    [BLOCK_WOOD] =
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 4, 4, 4, 4, 4, 4 }
    },
};

bool block_is_solid(enum BlockId id)
{
    return blocks[id].solid;
}

bool block_is_liquid(enum BlockId id)
{
    return id == BLOCK_WATER;
}

bool block_is_transparent(enum BlockId id)
{
    return blocks[id].transparent;
}

uint8_t block_get_tile_index(enum BlockId id, int face)
{
    assert(id != BLOCK_AIR);
    return blocks[id].tile_index[face];
}
