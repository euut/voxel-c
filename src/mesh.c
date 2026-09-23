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

static const float CUBE_UVS[8] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1
};

void mesh_init(struct ChunkMesh* mesh)
{
    memset(mesh, 0, sizeof(struct ChunkMesh));
    mesh->vertices = malloc(CHUNK_BLOCK_TOTAL * 24 * sizeof(struct Vertex));
    mesh->indices = malloc(CHUNK_BLOCK_TOTAL * 36 * sizeof(uint16_t));
}

void mesh_reset(struct ChunkMesh* mesh)
{
    mesh->vertex_count = 0;
    mesh->indices_count = 0;
}

void mesh_add_face(struct ChunkMesh* mesh, vec2s uv_offset, vec2s tile_unit, vec3s position, int face, int is_liquid)
{
    // Add 4 vertices for this face
    for (int i = 0; i < 4; i++)
    {
        vec3s v = CUBE_VERTICES[face][i];
        mesh->vertices[mesh->vertex_count+i].position.x = position.x + v.x;
        mesh->vertices[mesh->vertex_count+i].position.y = position.y + (is_liquid ? 0.9f : 1.0f) * v.y;
        mesh->vertices[mesh->vertex_count+i].position.z = position.z + v.z;
        mesh->vertices[mesh->vertex_count+i].uvs.x = uv_offset.x + tile_unit.x * CUBE_UVS[i*2 + 0];
        mesh->vertices[mesh->vertex_count+i].uvs.y = uv_offset.y + tile_unit.y * CUBE_UVS[i*2 + 1];

        // voxel shading
        switch (face)
        {
            case FACE_TOP:
                mesh->vertices[mesh->vertex_count+i].shading = 1.0f;
                break;
            case FACE_FRONT:
            case FACE_BACK:
                mesh->vertices[mesh->vertex_count+i].shading = 0.9f;
                break;
            case FACE_LEFT:
            case FACE_RIGHT:
                mesh->vertices[mesh->vertex_count+i].shading = 0.8f;
                break;
            default:
                mesh->vertices[mesh->vertex_count+i].shading = 0.6f;
                break;
        }
    }

    mesh->indices[mesh->indices_count++] = mesh->vertex_count + 0;
    mesh->indices[mesh->indices_count++] = mesh->vertex_count + 1;
    mesh->indices[mesh->indices_count++] = mesh->vertex_count + 2;
    mesh->indices[mesh->indices_count++] = mesh->vertex_count + 2;
    mesh->indices[mesh->indices_count++] = mesh->vertex_count + 3;
    mesh->indices[mesh->indices_count++] = mesh->vertex_count + 0;

    mesh->vertex_count += 4;
}

void mesh_upload(struct ChunkMesh* mesh)
{
    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(struct Vertex), mesh->vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &mesh->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_count * sizeof(uint16_t), mesh->indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    // uvs
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // shading
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(5 * sizeof(float)));
}

void mesh_render(struct ChunkMesh* mesh, struct Renderer* renderer)
{
    renderer_use_shader(renderer, SHADER_BASIC);
    shader_uniform_mat4(renderer->shader, "m", GLMS_MAT4_IDENTITY);
    shader_uniform_mat4(renderer->shader, "v", renderer->camera->view_matrix);
    shader_uniform_mat4(renderer->shader, "p", renderer->camera->projection_matrix);
    shader_uniform_texture(renderer->shader, "tex", renderer->texture_atlas.texture, 0);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void mesh_destroy(struct ChunkMesh* mesh)
{
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ibo);
    glDeleteVertexArrays(1, &mesh->vao);

    free(mesh->vertices);
    free(mesh->indices);
}