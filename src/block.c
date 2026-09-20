#include "block.h"

static struct Block blocks[BLOCK_TOTAL] =
{
    [BLOCK_AIR] = (struct Block)
    {
        .solid = false,
        .transparent = true
    },

    [BLOCK_DIRT] = (struct Block)
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 2, 2, 2, 2, 2, 2 }
    },

    [BLOCK_GRASS] = (struct Block)
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 3, 3, 3, 3, 0, 2 }
    },

    [BLOCK_STONE] = (struct Block)
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 1, 1, 1, 1, 1, 1 }
    },

    [BLOCK_SAND] = (struct Block)
    {
        .solid = true,
        .transparent = false,
        .tile_index = { 18, 18, 18, 18, 18, 18 }
    },

    [BLOCK_WATER] = (struct Block)
    {
        .solid = true, // temporarily set to solid
        .transparent = true,
        .tile_index = { 205, 205, 205, 205, 205, 205 }
    },

    [BLOCK_WOOD] = (struct Block)
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

bool block_is_transparent(enum BlockId id)
{
    return blocks[id].transparent;
}

uint16_t block_get_tile_index(enum BlockId id, int face)
{
    assert(id != BLOCK_AIR);
    return blocks[id].tile_index[face];
}
