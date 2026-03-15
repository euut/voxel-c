#include "shader.h"

static char* read_file(const char* path)
{
    FILE *f = fopen(path, "rb");

	if (f == NULL) {
		fprintf(stderr, "Failed to open file at: %s\n", path);
		exit(1);
	}
	
	fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *str = malloc(size + 1);
    fread(str, 1, size, f);
    str[size] = '\0';

	fclose(f);

	return str;
}

static unsigned int compile(const char* path, GLenum type)
{
    const char* shader_source = read_file(path);

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int log_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

        char* message = calloc(1, log_len);
        glGetShaderInfoLog(shader, log_len, NULL, message);
        fprintf(stderr, "Failed to compile shader at: %s, Error: %s\n", path, message);

        free(message);
    }

    free((void *)shader_source);

    return shader;
}


struct Shader shader_create(const char* vs_path, const char* fs_path)
{
    struct Shader shader;
	shader.id = glCreateProgram();

	unsigned int vertex_shader = compile(vs_path, GL_VERTEX_SHADER);
	unsigned int fragment_shader = compile(fs_path, GL_FRAGMENT_SHADER);

	glAttachShader(shader.id, vertex_shader);
	glAttachShader(shader.id, fragment_shader);

	glLinkProgram(shader.id);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader;
}

void shader_bind(struct Shader* shader)
{
    glUseProgram(shader->id);
}

void shader_destroy(struct Shader* shader)
{
    glDeleteProgram(shader->id);
}

void shader_uniform_mat4(struct Shader* shader, char *name, mat4s mat4)
{
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, (const GLfloat *) &mat4.raw);
}

void shader_uniform_texture(struct Shader* shader, char *name, struct Texture texture, GLuint n)
{
    texture_bind(texture);
    glActiveTexture(GL_TEXTURE0 + n);
    glUniform1i(glGetUniformLocation(shader->id, (const GLchar*) name), n);
}