#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

enum BlockId
{
    BLOCK_AIR = 0,
	BLOCK_DIRT,
	BLOCK_GRASS,
	BLOCK_STONE,
	BLOCK_SAND,
    BLOCK_WATER,
	BLOCK_WOOD,
    BLOCK_TOTAL
};

struct Block
{
    bool solid;
    bool transparent;
    uint16_t tile_index[6]; // texture location for each block face in the atlas
};

bool block_is_solid(enum BlockId id);
bool block_is_transparent(enum BlockId id);
uint16_t block_get_tile_index(enum BlockId id, int face);

#endif