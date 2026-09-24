#include "input.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    struct Input* input = glfwGetWindowUserPointer(window);

    if (key < 0 || key >= KEY_COUNT) return;

    if (action == GLFW_PRESS)
    {
        input->keys[key].down = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->keys[key].down = false;
    }
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    struct Input* input = glfwGetWindowUserPointer(window);

    if (button < 0 || button >= MOUSE_BUTTON_COUNT) return;

    if (action == GLFW_PRESS)
    {
        input->mouse[button].down = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->mouse[button].down = false;
    }
}

static void cursor_callback(GLFWwindow* window, double x, double y)
{
    struct Input* input = glfwGetWindowUserPointer(window);

    vec2s new_pos = {{x, y}};

    input->mouse_delta.x = new_pos.x - input->mouse_pos.x;
    input->mouse_delta.y = new_pos.y - input->mouse_pos.y;
    input->mouse_pos = new_pos;
}

void input_init(struct Input* input, struct Window* window)
{
    memset(input, 0, sizeof(struct Input));

    glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window->handle, input);
    glfwSetKeyCallback(window->handle, key_callback);
    glfwSetMouseButtonCallback(window->handle, mouse_callback);
    glfwSetCursorPosCallback(window->handle, cursor_callback);
}

void input_update(struct Input* input)
{
    for (int i = 0; i < KEY_COUNT; i++)
    {
        input->keys[i].pressed = input->keys[i].down && !input->keys[i].last;
        input->keys[i].last = input->keys[i].down;  // store state for next frame
    }

    for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
    {
        input->mouse[i].pressed = input->mouse[i].down && !input->mouse[i].last;
        input->mouse[i].last = input->mouse[i].down;
    }
}

bool input_key_down(struct Input* input, int key)
{
    return input->keys[key].down;
}

bool input_key_pressed(struct Input* input, int key)
{
    return input->keys[key].pressed;
}

bool input_mouse_down(struct Input* input, int button)
{
    return input->mouse[button].down;
}

bool input_mouse_pressed(struct Input* input, int button)
{
    return input->mouse[button].pressed;
}