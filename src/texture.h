#ifndef TEXTURE_H
#define TEXTURE_H

#include "util/math.h"

struct Texture
{
    unsigned int id;
    int width, height;
};

struct TextureAtlas
{
    struct Texture texture;
    vec2s tile_unit; // size of one tile in uv space
    ivec2s tiles; // number of tiles in x and y
};

struct Texture texture_load(const char* path);
void texture_bind(struct Texture texture);
void texture_destroy(struct Texture* texture);

struct TextureAtlas texture_atlas_create(const char* path, int tile_size);
vec2s texture_atlas_get_uv_offset(struct TextureAtlas* texture_atlas, uint16_t tile_index);
void texture_atlas_destroy(struct TextureAtlas* texture_atlas);

#endif