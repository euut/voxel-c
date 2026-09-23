#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

struct Window
{
    GLFWwindow* handle;
    int width, height;
};

void window_init(struct Window* window, int width, int height, const char* title);
void window_destroy(struct Window* window);

#endif