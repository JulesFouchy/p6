#include "Context.h"
#include <glad/glad.h>
#include <stdexcept>
#include <string>

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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    get_context(window).on_mouse_button(button, action, mods);
}
void scroll_callback(GLFWwindow* window, double x, double y)
{
    get_context(window).on_mouse_scroll(x, y);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    get_context(window).on_key(key, scancode, action, mods);
}

Context::Context(WindowCreationParams window_creation_params)
    : _window{window_creation_params}
    , _width{window_creation_params.width}
    , _height{window_creation_params.height}
{
    glfwSetWindowUserPointer(*_window, this);
    glfwSetWindowSizeCallback(*_window, &window_size_callback);
    glfwSetCursorPosCallback(*_window, &cursor_position_callback);
    glfwSetMouseButtonCallback(*_window, &mouse_button_callback);
    glfwSetScrollCallback(*_window, &scroll_callback);
    glfwSetKeyCallback(*_window, &key_callback);
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

/* ----------------------- *
 * ---------INPUT--------- *
 * ----------------------- */

glm::vec2 Context::mouse_position() const
{
    if (_mouse_position_is_initialized) {
        return _mouse_position;
    }
    else {
        double x, y; // NOLINT
        glfwGetCursorPos(*_window, &x, &y);
        return window_to_relative_coords({x, y});
    }
}

/* ------------------------ *
 * ---------WINDOW--------- *
 * ------------------------ */

float Context::aspect_ratio() const
{
    return static_cast<float>(_width) / static_cast<float>(_height);
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

/* ------------------------- *
 * ---------PRIVATE--------- *
 * ------------------------- */

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
    const auto delta = _mouse_position_is_initialized
                           ? pos - _mouse_position
                           : glm::vec2{0.f, 0.f};
    if (_is_dragging) {
        mouse_dragged({pos, delta, _drag_start_position});
    }
    else {
        mouse_moved({pos, delta});
    }
    _mouse_position                = pos;
    _mouse_position_is_initialized = true;
}

void Context::on_mouse_button(int button, int action, int /*mods*/)
{
    const auto mouse_button = [&]() {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            return Button::Left;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return Button::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return Button::Middle;
        default:
            throw std::runtime_error("[p6 internal error] Unknown mouse button: " + std::to_string(button));
        };
    }();
    const auto button_event = MouseButton{_mouse_position, mouse_button};
    if (action == GLFW_PRESS) {
        _is_dragging         = true;
        _drag_start_position = _mouse_position;
        mouse_pressed(button_event);
    }
    else if (action == GLFW_RELEASE) {
        _is_dragging = false;
        mouse_released(button_event);
    }
    else {
        throw std::runtime_error("[p6 internal error] Unknown mouse button action: " + std::to_string(action));
    }
}

void Context::on_mouse_scroll(double x, double y)
{
    mouse_scrolled({static_cast<float>(x),
                    static_cast<float>(y)});
}

void Context::on_key(int key, int scancode, int action, int /*mods*/)
{
    const char* key_name  = glfwGetKeyName(key, scancode);
    const auto  key_event = KeyEvent{key == GLFW_KEY_SPACE ? " " : key_name ? key_name
                                                                            : "",
                                    key};
    if (action == GLFW_PRESS) {
        key_pressed(key_event);
    }
    else if (action == GLFW_REPEAT) {
        key_repeated(key_event);
    }
    else if (action == GLFW_RELEASE) {
        key_released(key_event);
    }
    else {
        throw std::runtime_error("[p6 internal error] Unknown key action: " + std::to_string(action));
    }
}

} // namespace p6