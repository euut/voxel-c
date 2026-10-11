#ifndef INPUT_H
#define INPUT_H

#include "window.h"
#include "util/util.h"
#include <stdbool.h>
#include <string.h>

#define KEY_COUNT 512
#define MOUSE_BUTTON_COUNT 16

struct InputState
{
    bool down, pressed, last;
};

struct Input
{
    struct InputState keys[KEY_COUNT];
    struct InputState mouse[MOUSE_BUTTON_COUNT];
    vec2s mouse_pos;
    vec2s mouse_delta;
};

void input_init(struct Input* input, struct Window* window);
void input_update(struct Input* input);

static inline bool input_key_down(struct Input* input, int key)
{
    return input->keys[key].down;
}

static inline bool input_key_pressed(struct Input* input, int key)
{
    return input->keys[key].pressed;
}

static inline bool input_mouse_down(struct Input* input, int button)
{
    return input->mouse[button].down;
}

static inline bool input_mouse_pressed(struct Input* input, int button)
{
    return input->mouse[button].pressed;
}

#endif