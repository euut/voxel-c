#ifndef MESH_H
#define MESH_H

#include "util/math.h"
#include "renderer.h"
#include <glad/glad.h>
#include <string.h>
#include <stdint.h>

struct Chunk;

struct Mesh
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
};

void mesh_init(struct Mesh* mesh);
void mesh_reset(struct Mesh* mesh);
void mesh_add_face(struct Mesh* mesh, vec2s uv_offset, vec2s tile_unit, vec3s position, int face_index);
void mesh_upload(struct Mesh* mesh);
void mesh_render(struct Mesh* mesh, struct Renderer* renderer);
void mesh_destroy(struct Mesh* mesh);

#endif