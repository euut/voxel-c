#ifndef WORLDGEN_H
#define WORLDGEN_H

#include <fastnoiselite.h>
#include "chunk.h"

#define SEA_LEVEL (CHUNK_HEIGHT / 3)

void worldgen_init(int seed);
void worldgen_generate_terrain(struct Chunk* chunk);

#endif