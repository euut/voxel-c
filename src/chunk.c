#include "chunk.h"
#include "world.h"

static bool chunk_in_bound(ivec3s pos)
{
    return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < CHUNK_WIDTH && pos.y < CHUNK_HEIGHT && pos.z < CHUNK_WIDTH;
}

static bool chunk_on_bound(ivec3s pos)
{
    return pos.x == 0 || pos.y == 0 || pos.z == 0 || pos.x == (CHUNK_WIDTH - 1) || pos.y == (CHUNK_HEIGHT - 1) || pos.z == (CHUNK_WIDTH - 1);
}

void chunk_init(struct Chunk* chunk, struct World* world, ivec2s offset)
{
    memset(chunk, 0, sizeof(struct Chunk));

    chunk->offset = offset;
    chunk->world = world;
    chunk->blocks = calloc(CHUNK_BLOCK_TOTAL, sizeof(uint8_t));

    mesh_init(&chunk->opaque_mesh);
    mesh_init(&chunk->transparent_mesh);
}

uint8_t chunk_get_block(struct Chunk* chunk, ivec3s pos)
{
    if (!chunk_in_bound(pos)) {
        return BLOCK_AIR;
    }

    return chunk->blocks[chunk_block_index(pos)];
}

// Important! This cannot be called inside world_get_block()
uint8_t chunk_get_block2(struct Chunk* chunk, ivec3s pos)
{
    if (chunk_in_bound(pos)) {
        return chunk->blocks[chunk_block_index(pos)];
    }
    else {
        return world_get_block(chunk->world, chunk_local_to_world_pos(chunk->offset, pos));
    }
}

void chunk_set_block(struct Chunk* chunk, ivec3s pos, uint8_t block)
{
    chunk->blocks[chunk_block_index(pos)] = block;
    chunk->dirty = true;

    // mark affected neighbour chunks as dirty if updated block is on chunk edge
    if (chunk_on_bound(pos))
    {
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
}

bool should_render_face_between(enum BlockId block, enum BlockId neighbor)
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

static const ivec3s AO_OFFSETS[6][4][3] = {
    // +Z (front)
    {
        { (ivec3s){{ 1,  0, -1}}, (ivec3s){{ 0, -1, -1}}, (ivec3s){{ 1, -1, -1}} }, // Vertex 0
        { (ivec3s){{-1,  0, -1}}, (ivec3s){{ 0, -1, -1}}, (ivec3s){{-1, -1, -1}} }, // Vertex 1
        { (ivec3s){{-1,  0, -1}}, (ivec3s){{ 0,  1, -1}}, (ivec3s){{-1,  1, -1}} }, // Vertex 2
        { (ivec3s){{ 1,  0, -1}}, (ivec3s){{ 0,  1, -1}}, (ivec3s){{ 1,  1, -1}} }  // Vertex 3
    },
    // -Z (back)
    {
        { (ivec3s){{-1,  0,  1}}, (ivec3s){{ 0, -1,  1}}, (ivec3s){{-1, -1,  1}} }, // Vertex 0
        { (ivec3s){{ 1,  0,  1}}, (ivec3s){{ 0, -1,  1}}, (ivec3s){{ 1, -1,  1}} }, // Vertex 1
        { (ivec3s){{ 1,  0,  1}}, (ivec3s){{ 0,  1,  1}}, (ivec3s){{ 1,  1,  1}} }, // Vertex 2
        { (ivec3s){{-1,  0,  1}}, (ivec3s){{ 0,  1,  1}}, (ivec3s){{-1,  1,  1}} }  // Vertex 3
    },
    // +X (right)
    {
        { (ivec3s){{ 1,  0,  1}}, (ivec3s){{ 1, -1,  0}}, (ivec3s){{ 1, -1,  1}} }, // Vertex 0
        { (ivec3s){{ 1,  0, -1}}, (ivec3s){{ 1, -1,  0}}, (ivec3s){{ 1, -1, -1}} }, // Vertex 1
        { (ivec3s){{ 1,  0, -1}}, (ivec3s){{ 1,  1,  0}}, (ivec3s){{ 1,  1, -1}} }, // Vertex 2
        { (ivec3s){{ 1,  0,  1}}, (ivec3s){{ 1,  1,  0}}, (ivec3s){{ 1,  1,  1}} }  // Vertex 3
    },
    // -X (left)
    {
        { (ivec3s){{-1,  0, -1}}, (ivec3s){{-1, -1,  0}}, (ivec3s){{-1, -1, -1}} }, // Vertex 0
        { (ivec3s){{-1,  0,  1}}, (ivec3s){{-1, -1,  0}}, (ivec3s){{-1, -1,  1}} }, // Vertex 1
        { (ivec3s){{-1,  0,  1}}, (ivec3s){{-1,  1,  0}}, (ivec3s){{-1,  1,  1}} }, // Vertex 2
        { (ivec3s){{-1,  0, -1}}, (ivec3s){{-1,  1,  0}}, (ivec3s){{-1,  1, -1}} }  // Vertex 3
    },
    // +Y (top)
    {
        { (ivec3s){{-1,  1,  0}}, (ivec3s){{ 0,  1,  1}}, (ivec3s){{-1,  1,  1}} }, // Vertex 0
        { (ivec3s){{ 1,  1,  0}}, (ivec3s){{ 0,  1,  1}}, (ivec3s){{ 1,  1,  1}} }, // Vertex 1
        { (ivec3s){{ 1,  1,  0}}, (ivec3s){{ 0,  1, -1}}, (ivec3s){{ 1,  1, -1}} }, // Vertex 2
        { (ivec3s){{-1,  1,  0}}, (ivec3s){{ 0,  1, -1}}, (ivec3s){{-1,  1, -1}} }  // Vertex 3
    },
    // -Y (bottom)
    {
        { (ivec3s){{-1, -1,  0}}, (ivec3s){{ 0, -1, -1}}, (ivec3s){{-1, -1, -1}} }, // Vertex 0
        { (ivec3s){{ 1, -1,  0}}, (ivec3s){{ 0, -1, -1}}, (ivec3s){{ 1, -1, -1}} }, // Vertex 1
        { (ivec3s){{ 1, -1,  0}}, (ivec3s){{ 0, -1,  1}}, (ivec3s){{ 1, -1,  1}} }, // Vertex 2
        { (ivec3s){{-1, -1,  0}}, (ivec3s){{ 0, -1,  1}}, (ivec3s){{-1, -1,  1}} }  // Vertex 3
    }
};

void chunk_calculate_face_ao(struct Chunk* chunk, ivec3s pos, int face, int out_ao[4])
{
    for (int i = 0; i < 4; i++) {
        int s1 = block_is_solid(chunk_get_block2(chunk, glms_ivec3_add(pos, AO_OFFSETS[face][i][0])));
        int s2 = block_is_solid(chunk_get_block2(chunk, glms_ivec3_add(pos, AO_OFFSETS[face][i][1])));
        int c = block_is_solid(chunk_get_block2(chunk, glms_ivec3_add(pos, AO_OFFSETS[face][i][2])));
        
        if (s1 && s2) {
            out_ao[i] = 0; // fully occluded
        } else {
            out_ao[i] = 3 - (s1 + s2 + c); // soft shadow
        }
    }
}

void chunk_generate_mesh(struct Chunk* chunk, struct TextureAtlas* atlas)
{
    mesh_reset(&chunk->opaque_mesh);
    mesh_reset(&chunk->transparent_mesh);

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_WIDTH; z++)
            {
                ivec3s pos = {{x, y, z}};
                ivec3s world_pos = chunk_local_to_world_pos(chunk->offset, pos);

                enum BlockId block = chunk->blocks[chunk_block_index(pos)];
                enum BlockId neighbor_block;
                
                if (block == BLOCK_AIR) continue;
                
                for (int face = 0; face < 6; face++)
                {
                    ivec3s neighbor_pos = glms_ivec3_add(pos, FACE_NORMALS[face]);
                    ivec3s neighbor_world_pos = chunk_local_to_world_pos(chunk->offset, neighbor_pos);
                    
                    if (chunk_in_bound(neighbor_pos)) {
                        neighbor_block = chunk->blocks[chunk_block_index(neighbor_pos)];
                    }
                    else {
                        neighbor_block = world_get_block(chunk->world, neighbor_world_pos);
                    }
                    
                    if (should_render_face_between(block, neighbor_block))
                    {
                        uint8_t tile_index = block_get_tile_index(block, face);
                        vec2s uv_offset = texture_atlas_get_uv_offset(atlas, tile_index);

                        int ao[4];
                        chunk_calculate_face_ao(chunk, pos, face, ao);

                        // Pick which mesh (opaque or transparent) to send this face to
                        struct ChunkMesh* mesh = block_is_transparent(block) ? &chunk->transparent_mesh : &chunk->opaque_mesh;

                        mesh_add_face(mesh, uv_offset, atlas->tile_unit, ivec3s_to_vec3s(world_pos), face, block_is_liquid(block), ao);
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
        chunk_generate_mesh(chunk, &renderer->texture_atlas);
        chunk->dirty = false;
    }
    
    if (chunk->opaque_mesh.vertex_count > 0)
    {
        mesh_render(&chunk->opaque_mesh, renderer);
    }
}

void chunk_render_transparent(struct Chunk* chunk, struct Renderer* renderer)
{
    if (chunk->transparent_mesh.vertex_count > 0)
    {
        mesh_render(&chunk->transparent_mesh, renderer);
    }
}

void chunk_destroy(struct Chunk* chunk)
{
    mesh_destroy(&chunk->opaque_mesh);
    mesh_destroy(&chunk->transparent_mesh);
    free(chunk->blocks);
}