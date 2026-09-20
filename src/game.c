#include "game.h"

static void update(struct Game* game, float dt)
{
    input_update(&game->input);
    world_update(&game->world, &game->input);

    if (input_key_pressed(&game->input, GLFW_KEY_F))
    {
        renderer_enable_wireframe(&game->renderer);
    }

    game->input.mouse_delta = GLMS_VEC2_ZERO;
}

static void render(struct Game* game)
{
    renderer_clear(&game->renderer, 0.62f, 0.81f, 1.0f, 1.0f);

    world_render(&game->world, &game->renderer);
}

void game_init(struct Game* game)
{
    window_init(&game->window, 1920, 1080, "Minecraft", game);
    renderer_init(&game->renderer);
    input_init(&game->input, &game->window);
    world_init(&game->world, 0);
}

void game_run(struct Game* game)
{
    double last_frame = glfwGetTime();
    double accumulator = 0.0;
    double timer = glfwGetTime();
    const double SECONDS_PER_TICK = 1.0 / 60.0; // 60 UPS

    int updates = 0;
    int frames = 0;

    while (!glfwWindowShouldClose(game->window.handle))
	{
        double now = glfwGetTime();
        double delta_time = now - last_frame;
        last_frame = now;
        
        accumulator += delta_time;

        glfwPollEvents();
        
        while (accumulator >= SECONDS_PER_TICK)
        {
            update(game, SECONDS_PER_TICK);
            updates++;
            accumulator -= SECONDS_PER_TICK;
        }

        render(game);
        frames++;

        if (now - timer >= 1.0)
        {
            char title[64];
            snprintf(title, sizeof(title), "Minecraft | %d fps, %d ups", frames, updates);
            glfwSetWindowTitle(game->window.handle, title);
            
            updates = 0;
            frames = 0;
            timer += 1.0;
        }

		glfwSwapBuffers(game->window.handle);
	}
}

void game_shutdown(struct Game* game)
{
    world_destroy(&game->world);
    renderer_destroy(&game->renderer);
    window_destroy(&game->window);
}