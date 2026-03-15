#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"

struct Renderer
{
    struct Shader shaders[SHADER_TOTAL];
    struct Shader* shader;
    struct Camera* camera;
    struct TextureAtlas texture_atlas;
    bool wireframe;
};

void renderer_init(struct Renderer* renderer);
void renderer_use_shader(struct Renderer* renderer, enum ShaderType type);
void renderer_enable_wireframe(struct Renderer* renderer);
void renderer_submit_mesh(struct Renderer* renderer, struct Mesh* mesh);
void renderer_draw_mesh(struct Renderer* renderer, struct Mesh* mesh);
void renderer_clear(struct Renderer* renderer, float r, float g, float b, float a);
void renderer_destroy(struct Renderer* renderer);

#endif