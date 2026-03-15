#ifndef SHADER_H
#define SHADER_H

#include "texture.h"
#include "util/math.h"

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum ShaderType
{
    SHADER_BASIC = 0,
    SHADER_TOTAL
};

struct Shader
{
    unsigned int id;
};

struct Shader shader_create(const char* vs_path, const char* fs_path);
void shader_bind(struct Shader* shader);
void shader_destroy(struct Shader* shader);
void shader_uniform_mat4(struct Shader* shader, char *name, mat4s mat4);
void shader_uniform_texture(struct Shader* shader, char *name, struct Texture texture, GLuint n);

#endif