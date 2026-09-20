#include "chunk.h"
#include "world.h"

static inline bool chunk_in_bound(ivec3s pos)
{
    return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < CHUNK_WIDTH && pos.y < CHUNK_HEIGHT && pos.z < CHUNK_WIDTH;
}

static inline bool chunk_on_bound(ivec3s pos)
{
    return pos.x == 0 || pos.y == 0 || pos.z == 0 || pos.x == (CHUNK_WIDTH - 1) || pos.y == (CHUNK_HEIGHT - 1) || pos.z == (CHUNK_WIDTH - 1);
}

void chunk_init(struct Chunk* chunk, struct World* world, ivec2s offset)
{
    memset(chunk, 0, sizeof(struct Chunk));

    chunk->offset = offset;
    chunk->world = world;
    chunk->blocks = malloc(CHUNK_BLOCK_TOTAL * sizeof(uint8_t));

    mesh_init(&chunk->mesh);
}

uint8_t chunk_get_block(struct Chunk* chunk, ivec3s pos)
{
    if (!chunk_in_bound(pos)) return BLOCK_AIR;

    return chunk->blocks[chunk_block_index(pos)];
}

void chunk_set_block(struct Chunk* chunk, ivec3s pos, uint8_t block_id)
{
    chunk->blocks[chunk_block_index(pos)] = block_id;
    chunk->dirty = true;

    if (!chunk_on_bound(pos)) return;

    // mark affected neighbour chunks as dirty if updated block is on chunk edge
    struct Chunk* neighbor;

    if (pos.x == 0)
    {
        neighbor = world_get_chunk(chunk->world, glms_ivec2_add(chunk->offset, (ivec2s) {{ -1, 0 }}));
        
        if (neighbor != NULL) neighbor->dirty = true;
    }

    if (pos.x == CHUNK_WIDTH - 1)
    {
        neighbor = world_get_chunk(chunk->world, glms_ivec2_add(chunk->offset, (ivec2s) {{ 1, 0 }}));

        if (neighbor != NULL) neighbor->dirty = true;
    } 

    if (pos.z == 0)
    {
        neighbor = world_get_chunk(chunk->world, glms_ivec2_add(chunk->offset, (ivec2s) {{ 0, -1 }}));

        if (neighbor != NULL) neighbor->dirty = true;
    }

    if (pos.z == CHUNK_WIDTH - 1)
    {
        neighbor = world_get_chunk(chunk->world, glms_ivec2_add(chunk->offset, (ivec2s) {{ 0, 1 }}));

        if (neighbor != NULL) neighbor->dirty = true;
    }
}

void chunk_build_mesh(struct Chunk* chunk, struct TextureAtlas* atlas)
{
    mesh_reset(&chunk->mesh);

    static const ivec3s CUBE_FACE_NORMALS[6] =
    {
        {{  0,  0, -1 }},
        {{  0,  0,  1 }},
        {{  1,  0,  0 }},
        {{ -1,  0,  0 }},
        {{  0,  1,  0 }},
        {{  0, -1,  0 }},
    };

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_WIDTH; z++)
            {
                ivec3s pos = {{x, y, z}};
                ivec3s world_pos = chunk_local_to_world(chunk->offset, pos);

                enum BlockId block = chunk_get_block(chunk, pos);
                enum BlockId neighbor_block;
                
                if (block == BLOCK_AIR) continue;
                
                for (int face = 0; face < 6; face++)
                {
                    ivec3s neighbor_pos = glms_ivec3_add(pos, CUBE_FACE_NORMALS[face]);
                    ivec3s neighbor_world_pos = glms_ivec3_add(world_pos, CUBE_FACE_NORMALS[face]);

                    if (chunk_in_bound(neighbor_pos))
                    {
                        neighbor_block = chunk_get_block(chunk, neighbor_pos);
                    }
                    else
                    {
                        neighbor_block = world_get_block(chunk->world, neighbor_world_pos);
                    }
                    
                    if (!neighbor_block || !block_is_solid(neighbor_block))
                    {
                        uint16_t tile_index = block_get_tile_index(block, face);
                        vec2s uv_offset = texture_atlas_get_uv_offset(atlas, tile_index);

                        mesh_add_face(&chunk->mesh, uv_offset, atlas->tile_unit, ivec3s_to_vec3s(world_pos), face);
                    }
                }
            }
        }
    }

    mesh_upload(&chunk->mesh);
}

void chunk_render(struct Chunk* chunk, struct Renderer* renderer)
{
    if (chunk->dirty)
    {
        chunk_build_mesh(chunk, &renderer->texture_atlas);
        chunk->dirty = false;
    }
    
    mesh_render(&chunk->mesh, renderer);
}

void chunk_destroy(struct Chunk* chunk)
{
    mesh_destroy(&chunk->mesh);
    free(chunk->blocks);
}