#define FNL_IMPL
#include "worldgen.h"
#include <math.h>

static fnl_state terrain_noise;

void worldgen_init(int seed)
{
    terrain_noise = fnlCreateState();
    terrain_noise.seed = seed;
    terrain_noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    terrain_noise.frequency = 0.005f;
    terrain_noise.fractal_type = FNL_FRACTAL_FBM;
    terrain_noise.octaves = 5;
    terrain_noise.lacunarity = 2.0f;
    terrain_noise.gain = 0.5f;
}

void worldgen_generate_terrain(struct Chunk* chunk)
{
    for (int z = 0; z < CHUNK_WIDTH; z++)
    {
        for (int x = 0; x < CHUNK_WIDTH; x++)
        {
            ivec3s world_pos = chunk_local_to_world(chunk->offset, (ivec3s){{ x, 0, z }});

            // Sample noise
            float noise = fnlGetNoise2D(&terrain_noise, (float)world_pos.x, (float)world_pos.z);

            // Normalize
            float h = (noise + 1.0f) * 0.5f;

            // float h = (n * 0.6f) + (n * n * 0.4f);

            int terrain_height = (int)(h * CHUNK_HEIGHT);

            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                enum BlockId block;

                if (y > terrain_height)
                {
                    if (y <= SEA_LEVEL)
                        block = BLOCK_WATER;
                    else
                        block = BLOCK_AIR;
                }
                else
                {
                    if (terrain_height >= SEA_LEVEL - 2 && terrain_height <= SEA_LEVEL + 1)
                    {
                        // Sand near water
                        if (y == terrain_height)
                            block = BLOCK_SAND;
                        else if (y > terrain_height - 3)
                            block = BLOCK_SAND;
                        else
                            block = BLOCK_STONE;
                    }
                    else if (h > 0.65f)
                    {
                        // Mountains
                        if (y > terrain_height - 2)
                            block = BLOCK_GRASS;
                        else
                            block = BLOCK_STONE;
                    }
                    else
                    {
                        // Sand underwater
                        if (terrain_height < SEA_LEVEL)
                        {
                            if (y == terrain_height || y > terrain_height - 3)
                                block = BLOCK_SAND;
                            else
                                block = BLOCK_STONE;
                        }
                        // Plains
                        else
                        {
                            if (y == terrain_height)
                                block = BLOCK_GRASS;
                            else if (y > terrain_height - 3)
                                block = BLOCK_DIRT;
                            else
                                block = BLOCK_STONE;
                        }
                    }
                }

                chunk_set_block(chunk, (ivec3s){{ x, y, z }}, block);
            }
        }
    }
}