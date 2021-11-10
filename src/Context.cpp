#include "Context.h"
#include <glad/glad.h>

namespace p6 {

Context::Context(WindowCreationParams window_creation_params)
    : _window{window_creation_params}
{
}

void Context::run()
{
    while (!glfwWindowShouldClose(*_window)) {
        glClearColor(0.1f, 0.8f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(*_window);
        glfwPollEvents();
    }
}

} // namespace p6