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
        update();
        glfwSwapBuffers(*_window);
        glfwPollEvents();
        _clock->update();
    }
}

void Context::background(Color color) const
{
    glClearColor(color.r(), color.g(), color.b(), color.a());
    glClear(GL_COLOR_BUFFER_BIT);
}

float Context::time() const
{
    return _clock->time();
}

float Context::delta_time() const
{
    return _clock->delta_time();
}

} // namespace p6