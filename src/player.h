#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "input.h"
#include "block.h"

struct World;

struct Player
{
    struct Camera camera;
    struct World* world;

    float speed;
    vec3s position;    
    
    enum BlockId selected_block;
};

void player_init(struct Player* player, vec3s position, struct World* world);
void player_update(struct Player* player, struct Input* input);

#endif