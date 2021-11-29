#include "Context.h"
#include <glad/glad.h>
#include <glm/gtx/matrix_transform_2d.hpp>
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
    , _mouse_position{compute_mouse_position()}
    , _render_target{{window_creation_params.width, window_creation_params.height}}
{
    glpp::set_error_callback([&](std::string&& error_message) { // TODO glpp's error callback is global while on_error is tied to a context. This means that if we create two Contexts glpp will only use the error callback of the second Context.
        on_error(std::move(error_message));
    });
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    glfwSetWindowUserPointer(*_window, this);
    glfwSetWindowSizeCallback(*_window, &window_size_callback);
    glfwSetMouseButtonCallback(*_window, &mouse_button_callback);
    glfwSetScrollCallback(*_window, &scroll_callback);
    glfwSetKeyCallback(*_window, &key_callback);

    _render_target.bind();
}

void Context::run()
{
    while (!glfwWindowShouldClose(*_window)) {
        _render_target.bind();
        check_for_mouse_movements();
        if (is_looping()) {
            update();
        }
        glpp::bind_framebuffer_as_draw(0);
        glpp::bind_framebuffer_as_read(*_render_target.framebuffer());
        glpp::blit_framebuffer(0, 0, _width, _height,
                               0, 0, _width, _height,
                               GL_COLOR_BUFFER_BIT, glpp::Interpolation::NearestNeighbour);
        glpp::bind_framebuffer(0);
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

void Context::rectangle(RectangleParams params) const
{
    render_with_rect_shader(params, false);
}

void Context::ellipse(RectangleParams params) const
{
    render_with_rect_shader(params, true);
}

void Context::render_with_rect_shader(RectangleParams params, bool is_ellipse) const
{
    _rect_shader.bind();
    _rect_shader.set("_is_ellipse", is_ellipse);
    _rect_shader.set("_inverse_aspect_ratio", 1.f / aspect_ratio());
    _rect_shader.set("_transform", glm::scale(glm::rotate(glm::translate(glm::mat3{1.f},
                                                                         params.position),
                                                          params.rotation),
                                              params.size));
    _rect_shader.set("_rect_size", params.size);
    _rect_shader.set("_fill_color", fill.as_vec4());
    _rect_shader.set("_stroke_color", stroke.as_vec4());
    _rect_shader.set("_stroke_weight", stroke_weight);
    _rect_renderer.render();
}
/* -------------------------------- *
 * ---------RENDER TARGETS--------- *
 * -------------------------------- */

void Context::set_render_target(const RenderTarget& render_target) const
{
    (void)render_target;
}

void Context::reset_render_target() const
{
    // _render_target.;
}

/* ----------------------- *
 * ---------INPUT--------- *
 * ----------------------- */

glm::vec2
    Context::mouse() const
{
    return _mouse_position;
}

glm::vec2 Context::mouse_delta() const
{
    return _mouse_position_delta;
}

bool Context::mouse_is_in_window() const
{
    if (!window_is_focused()) {
        return false;
    }
    const auto pos = mouse();
    return pos.x >= -aspect_ratio() && pos.x <= aspect_ratio() &&
           pos.y >= -1.f && pos.y <= 1.f;
}

bool Context::ctrl() const
{
    return glfwGetKey(*_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
           glfwGetKey(*_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
}

bool Context::shift() const
{
    return glfwGetKey(*_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
           glfwGetKey(*_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

bool Context::alt() const
{
    return glfwGetKey(*_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
           glfwGetKey(*_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
}

/* ------------------------ *
 * ---------WINDOW--------- *
 * ------------------------ */

float Context::aspect_ratio() const
{
    return static_cast<float>(_width) / static_cast<float>(_height);
}

ImageSize Context::window_size() const
{
    return {_width, _height};
}

bool Context::window_is_focused() const
{
    return static_cast<bool>(glfwGetWindowAttrib(*_window, GLFW_FOCUSED));
}

void Context::maximize_window()
{
    glfwMaximizeWindow(*_window);
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
    _render_target.resize({width, height});
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

void Context::on_mouse_move()
{
    if (_is_dragging) {
        mouse_dragged({mouse(), mouse_delta(), _drag_start_position});
    }
    else {
        mouse_moved({mouse(), mouse_delta()});
    }
}

void Context::check_for_mouse_movements()
{
    const auto mouse_pos = compute_mouse_position();
    if (mouse_pos != _mouse_position) {
        _mouse_position_delta = mouse_pos - _mouse_position;
        _mouse_position       = mouse_pos;
        if (window_is_focused()) {
            on_mouse_move();
        }
    }
    else {
        _mouse_position_delta = glm::vec2{0.f};
    }
}

glm::vec2 Context::compute_mouse_position() const
{
    double x, y; // NOLINT
    glfwGetCursorPos(*_window, &x, &y);
    return window_to_relative_coords({x, y});
}

} // namespace p6