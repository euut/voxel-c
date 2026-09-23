#ifndef MESH_H
#define MESH_H

#include "util/math.h"
#include "renderer.h"
#include <glad/glad.h>
#include <string.h>
#include <stdint.h>

struct Chunk;

enum Face
{
    FACE_FRONT = 0,
    FACE_BACK,
    FACE_RIGHT,
    FACE_LEFT,
    FACE_TOP,
    FACE_BOTTOM
};

struct ChunkMesh
{
    struct Vertex* vertices;
    uint16_t* indices;

    size_t vertex_count;
    size_t indices_count;

    unsigned int vao, vbo, ibo;
};

struct Vertex
{
    vec3s position;
    vec2s uvs;
    float shading;
};

void mesh_init(struct ChunkMesh* mesh);
void mesh_reset(struct ChunkMesh* mesh);
void mesh_add_face(struct ChunkMesh* mesh, vec2s uv_offset, vec2s tile_unit, vec3s position, int face_index, int is_liquid);
void mesh_upload(struct ChunkMesh* mesh);
void mesh_render(struct ChunkMesh* mesh, struct Renderer* renderer);
void mesh_destroy(struct ChunkMesh* mesh);

#endif