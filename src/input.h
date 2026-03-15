#ifndef INPUT_H
#define INPUT_H

#include "window.h"
#include "util/math.h"
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
bool input_key_down(struct Input* input, int key);
bool input_key_pressed(struct Input* input, int key);
bool input_mouse_down(struct Input* input, int button);
bool input_mouse_pressed(struct Input* input, int button);

#endif