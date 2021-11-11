#include "Context.h"
#include <glad/glad.h>

namespace p6 {

static Context& get_context(GLFWwindow* window)
{
    return *reinterpret_cast<p6::Context*>(glfwGetWindowUserPointer(window)); // NOLINT
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    get_context(window).on_window_resize(width, height);
}

Context::Context(WindowCreationParams window_creation_params)
    : _window{window_creation_params}
    , _width{window_creation_params.width}
    , _height{window_creation_params.height}
{
    glfwSetWindowUserPointer(*_window, this);
    glfwSetWindowSizeCallback(*_window, &window_size_callback);
}

void Context::run()
{
    while (!glfwWindowShouldClose(*_window)) {
        if (is_looping()) {
            update();
        }
        glfwSwapBuffers(*_window);
        glfwPollEvents();
        _clock->update();
    }
}

void Context::on_window_resize(int width, int height)
{
    _width  = width;
    _height = height;
    glViewport(0, 0, width, height);
}

/* ------------------------- *
 * ---------DRAWING--------- *
 * ------------------------- */

void Context::background(Color color) const
{
    glClearColor(color.r(), color.g(), color.b(), color.a());
    glClear(GL_COLOR_BUFFER_BIT);
}

void Context::rect() const
{
    _rect_renderer.render();
}

/* ---------------------- *
 * ---------TIME--------- *
 * ---------------------- */

float Context::time() const
{
    return _clock->time();
}

float Context::delta_time() const
{
    return _clock->delta_time();
}

void Context::set_time_mode_realtime()
{
    const auto t          = _clock->time();
    const auto was_paused = !_clock->is_playing();
    _clock                = std::make_unique<details::Clock_Realtime>();
    _clock->set_time(t);
    if (was_paused) {
        _clock->pause();
    }
}

void Context::set_time_mode_fixedstep()
{
    const auto t          = _clock->time();
    const auto was_paused = !_clock->is_playing();
    _clock                = std::make_unique<details::Clock_FixedTimestep>(60.f);
    _clock->set_time(t);
    if (was_paused) {
        _clock->pause();
    }
}

/* ------------------------------- *
 * ---------MISCELLANEOUS--------- *
 * ------------------------------- */

void Context::loop()
{
    _clock->play();
}

void Context::no_loop()
{
    _clock->pause();
}

bool Context::is_looping() const
{
    return _clock->is_playing();
}

} // namespace p6