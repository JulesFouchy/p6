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

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    get_context(window).on_mouse_move(x, y);
}

Context::Context(WindowCreationParams window_creation_params)
    : _window{window_creation_params}
    , _width{window_creation_params.width}
    , _height{window_creation_params.height}
{
    glfwSetWindowUserPointer(*_window, this);
    glfwSetWindowSizeCallback(*_window, &window_size_callback);
    glfwSetCursorPosCallback(*_window, &cursor_position_callback);
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

glm::vec2 Context::window_to_relative_coords(glm::vec2 pos) const
{
    const auto w = static_cast<float>(_width);
    const auto h = static_cast<float>(_height);

    pos.y = h - pos.y;    // Make y-axis point up
    pos.x -= w / 2.f;     // Center around 0
    pos.y -= h / 2.f;     // Center around 0
    return pos / h * 2.f; // Normalize
}

void Context::on_window_resize(int width, int height)
{
    _width  = width;
    _height = height;
    glViewport(0, 0, width, height);
}

void Context::on_mouse_move(double x, double y)
{
    const auto pos   = window_to_relative_coords({x, y});
    const auto delta = _previous_position_is_initialized
                           ? pos - _previous_position
                           : glm::vec2{0.f, 0.f};
    if (_is_dragging) {
        mouse_dragged({pos, delta, _drag_start_position});
    }
    else {
        mouse_moved({pos, delta});
    }
    _previous_position                = pos;
    _previous_position_is_initialized = true;
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