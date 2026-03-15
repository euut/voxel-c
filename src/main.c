#include "game.h"

int main()
{
    struct Game game;

    game_init(&game);
    game_run(&game);
    game_shutdown(&game);
}