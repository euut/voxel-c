#ifndef MESH_H
#define MESH_H

#include "renderer.h"
#include "util/util.h"
#include <glad/glad.h>
#include <string.h>
#include <stdint.h>

struct Vertex
{
    vec3s position;
    vec3s normal;
    vec2s uv;
    float ao; // ambient occlusion
};

struct ChunkMesh
{
    struct Vertex* vertices;
    uint16_t* indices;

    size_t vertex_count;
    size_t index_count;

    unsigned int vao, vbo, ibo;
};

void mesh_init(struct ChunkMesh* mesh);
void mesh_reset(struct ChunkMesh* mesh);
void mesh_add_face(struct ChunkMesh* mesh, vec2s uv_offset, vec2s tile_unit, vec3s position, int face, int is_liquid, int ao[4]);
void mesh_upload(struct ChunkMesh* mesh);
void mesh_render(struct ChunkMesh* mesh, struct Renderer* renderer);
void mesh_destroy(struct ChunkMesh* mesh);

#endif