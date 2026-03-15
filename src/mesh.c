#include "mesh.h"
#include "chunk.h"

static const vec3s CUBE_VERTICES[6][4] = {
    { { 1, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 } }, // +Z (front)
    { { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 } }, // -Z (back)
    { { 1, 0, 1 }, { 1, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 } }, // +X (right)
    { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 } }, // -X (left)
    { { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 }, { 0, 1, 0 } }, // +Y (top)
    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 } } // -Y (bottom)
};

static const uint16_t FACE_INDICES[6] = {
    0, 1, 2,
    2, 3, 0
};

static const float CUBE_UVS[8] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1
};

void mesh_init(struct Mesh* mesh)
{
    memset(mesh, 0, sizeof(struct Mesh));
    mesh->vertices = malloc(CHUNK_BLOCK_TOTAL * 24 * sizeof(struct Vertex));
    mesh->indices = malloc(CHUNK_BLOCK_TOTAL * 36 * sizeof(uint16_t));
}

void mesh_reset(struct Mesh* mesh)
{
    mesh->vertex_count = 0;
    mesh->indices_count = 0;
}

void mesh_add_face(struct Mesh* mesh, vec2s uv_offset, vec2s tile_unit, vec3s position, int face)
{
    // Add 4 vertices for this face
    for (int i = 0; i < 4; i++)
    {
        vec3s v = CUBE_VERTICES[face][i];
        mesh->vertices[mesh->vertex_count+i].position.x = position.x + v.x;
        mesh->vertices[mesh->vertex_count+i].position.y = position.y + v.y;
        mesh->vertices[mesh->vertex_count+i].position.z = position.z + v.z;
        mesh->vertices[mesh->vertex_count+i].uvs.x = uv_offset.x + tile_unit.x * CUBE_UVS[i*2 + 0];
        mesh->vertices[mesh->vertex_count+i].uvs.y = uv_offset.y + tile_unit.y * CUBE_UVS[i*2 + 1];
    }

    // Add indices for two triangles
    for (int i = 0; i < 6; i++)
    {
        mesh->indices[mesh->indices_count++] = mesh->vertex_count + FACE_INDICES[i];
    }

    mesh->vertex_count += 4;
}

void mesh_destroy(struct Mesh* mesh)
{
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ibo);
    glDeleteVertexArrays(1, &mesh->vao);

    free(mesh->vertices);
    free(mesh->indices);
}