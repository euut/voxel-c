#include "renderer.h"

void renderer_init(struct Renderer* renderer)
{
    memset(renderer, 0, sizeof(struct Renderer));

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // texture atlas
    renderer->texture_atlas = texture_atlas_create("res/textures/atlas.png", 16);

    // shaders
    renderer->shaders[SHADER_BASIC] = shader_create("res/shaders/basic.vert", "res/shaders/basic.frag");
}

void renderer_use_shader(struct Renderer* renderer, enum ShaderType type)
{
    assert(type >= 0 && type < SHADER_TOTAL);

    struct Shader* shader = &renderer->shaders[type];

    renderer->shader = shader;
    shader_bind(shader);
}

void renderer_enable_wireframe(struct Renderer* renderer)
{
    renderer->wireframe = !renderer->wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, renderer->wireframe ? GL_LINE : GL_FILL);
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