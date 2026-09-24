#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Window
{
    GLFWwindow* handle;
    int width, height;
    bool fullscreen;
};

void window_init(struct Window* window, int width, int height, const char* title);
void window_toggle_fullscreen(struct Window* window);
void window_destroy(struct Window* window);

#endif