#include "renderer.h"

void renderer_init(struct Renderer* renderer)
{
    memset(renderer, 0, sizeof(struct Renderer));

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    renderer->wireframe = false;
    
    // texture atlas
    renderer->texture_atlas = texture_atlas_create("res/textures/atlas.png", 16);

    // shaders
    renderer->shaders[SHADER_BASIC] = shader_create("res/shaders/basic.vert", "res/shaders/basic.frag");
}

void renderer_use_shader(struct Renderer* renderer, enum ShaderType type)
{
    renderer->shader = &renderer->shaders[type];
    shader_bind(renderer->shader);
}

void renderer_enable_wireframe(struct Renderer* renderer)
{
    renderer->wireframe = !renderer->wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, renderer->wireframe ? GL_LINE : GL_FILL);
}

void renderer_submit_mesh(struct Renderer* renderer, struct Mesh* mesh)
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
}

void renderer_draw_mesh(struct Renderer* renderer, struct Mesh* mesh)
{
    renderer_use_shader(renderer, SHADER_BASIC);
    shader_uniform_mat4(renderer->shader, "m", GLMS_MAT4_IDENTITY);
    shader_uniform_mat4(renderer->shader, "v", renderer->camera->view_matrix);
    shader_uniform_mat4(renderer->shader, "p", renderer->camera->projection_matrix);
    shader_uniform_texture(renderer->shader, "tex", renderer->texture_atlas.texture, 0);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, 0);
}

void renderer_clear(struct Renderer* renderer, float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_destroy(struct Renderer* renderer)
{
    texture_atlas_destroy(&renderer->texture_atlas);

    for (size_t i = 0; i < SHADER_TOTAL; i++)
    {
        shader_destroy(&renderer->shaders[i]);
    }
}