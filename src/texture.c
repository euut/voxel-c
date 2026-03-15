#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.h"
#include <glad/glad.h>

struct Texture texture_load(const char* path)
{
    struct Texture texture;
    
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

    if (!data) {
        fprintf(stderr, "Failed to load texture at: %s, Error: %s\n", path, stbi_failure_reason());
    }

	texture.width = width;
	texture.height = height;

    glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture.id);

	stbi_image_free(data);

	return texture;
}

void texture_bind(struct Texture texture)
{
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

void texture_destroy(struct Texture* texture)
{
    glDeleteTextures(1, &texture->id);
}

struct TextureAtlas texture_atlas_create(const char* path, int tile_size)
{
	struct TextureAtlas atlas;

	atlas.texture = texture_load(path);

    atlas.tile_unit.x = (float)tile_size / (float)atlas.texture.width;
    atlas.tile_unit.y = (float)tile_size / (float)atlas.texture.height;
	
    atlas.tiles.x = atlas.texture.width / tile_size;
    atlas.tiles.y = atlas.texture.height / tile_size;

	return atlas;
}

vec2s texture_atlas_get_uv_offset(struct TextureAtlas* atlas, uint16_t tile_index)
{
	int col = tile_index % atlas->tiles.x;
	int row = tile_index / atlas->tiles.x;

    float u = col * atlas->tile_unit.x;
    float v = 1.0f - ((row + 1) * atlas->tile_unit.y); // flip v for bottom-left origin

    return (vec2s){{ u, v }};
}

void texture_atlas_destroy(struct TextureAtlas* atlas)
{
	texture_destroy(&atlas->texture);
}