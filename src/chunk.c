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

    mesh_init(&chunk->opaque_mesh);
    mesh_init(&chunk->transparent_mesh);
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

static bool should_render_face_between(enum BlockId block, enum BlockId neighbor)
{
    // air next to anything, render face
    if (neighbor == BLOCK_AIR) {
        return true;
    }

    // opaque next to transparent, render face
    if (!block_is_transparent(block) && block_is_transparent(neighbor)) {
        return true;
    }

    // transparent next to transparent but same type, hide face
    if (block == neighbor && block_is_transparent(block)) {
        return false;
    }

    // transparent next to transparent but different type, render face
    if (block_is_transparent(block) && block_is_transparent(neighbor)) {
        return true;
    }

    // opaque next to opaque, hide face
    return false;
}

void chunk_build_mesh(struct Chunk* chunk, struct TextureAtlas* atlas)
{
    mesh_reset(&chunk->opaque_mesh);
    mesh_reset(&chunk->transparent_mesh);

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
                    
                    uint8_t tile_index = block_get_tile_index(block, face);
                    vec2s uv_offset = texture_atlas_get_uv_offset(atlas, tile_index);

                    if (chunk_in_bound(neighbor_pos)) {
                        neighbor_block = chunk->blocks[chunk_block_index(neighbor_pos)];
                    }
                    else {
                        neighbor_block = world_get_block(chunk->world, neighbor_world_pos);
                    }
                    
                    if (should_render_face_between(block, neighbor_block))
                    {
                        // Pick which mesh (opaque or transparent) to send this face to
                        struct ChunkMesh* target_mesh = block_is_transparent(block) ? &chunk->transparent_mesh : &chunk->opaque_mesh;

                        mesh_add_face(target_mesh, uv_offset, atlas->tile_unit, ivec3s_to_vec3s(world_pos), face, block_is_liquid(block));
                    }
                }
            }
        }
    }

    // upload mesh data to GPU
    mesh_upload(&chunk->opaque_mesh);
    mesh_upload(&chunk->transparent_mesh);
}

void chunk_render_opaque(struct Chunk* chunk, struct Renderer* renderer)
{
    if (chunk->dirty)
    {
        chunk_build_mesh(chunk, &renderer->texture_atlas);
        chunk->dirty = false;
    }
    
    mesh_render(&chunk->opaque_mesh, renderer);
}

void chunk_render_transparent(struct Chunk* chunk, struct Renderer* renderer)
{
    mesh_render(&chunk->transparent_mesh, renderer);
}

void chunk_destroy(struct Chunk* chunk)
{
    mesh_destroy(&chunk->opaque_mesh);
    mesh_destroy(&chunk->transparent_mesh);
    free(chunk->blocks);
}