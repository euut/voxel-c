#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "renderer.h"
#include "input.h"
#include "world.h"

struct Game
{
    struct Window window;
    struct Renderer renderer;
    struct Input input;
    struct World world;
};

void game_init(struct Game* game);
void game_run(struct Game* game);
void game_shutdown(struct Game* game);

#endif