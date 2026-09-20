#include "window.h"

static void resize_callback(GLFWwindow* handle, int width, int height)
{
    glViewport(0, 0, width, height);
}

void window_init(struct Window* window, int width, int height, const char* title, void* game)
{
    if (!glfwInit())
    {
        fprintf(stderr, "%s",  "Failed to initialize GLFW\n");
        exit(1);
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
    window->width = width;
    window->height = height;

    glfwMakeContextCurrent(window->handle);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        exit(1);
    }

	// Get size of monitor and set the window to the middle of the screen
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window->handle, (mode->width - width) / 2, (mode->height - height) / 2);

    glfwSetFramebufferSizeCallback(window->handle, resize_callback);
    
	glfwSwapInterval(1);
}

void window_destroy(struct Window* window)
{
    glfwDestroyWindow(window->handle);
    glfwTerminate();
}