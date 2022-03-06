#include "Context.h"
#include <glad/glad.h>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <stdexcept>
#include <string>
#include "math.h"

namespace p6 {

static Context& get_context(GLFWwindow* window)
{
    return *reinterpret_cast<p6::Context*>(glfwGetWindowUserPointer(window)); // NOLINT
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    get_context(window).on_framebuffer_resize(width, height);
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
    , _window_size{window_creation_params.width,
                   window_creation_params.height}
    , _mouse_position{compute_mouse_position()}
    , _default_canvas{{1, 1}}
{
    glpp::set_error_callback([&](std::string&& error_message) { // TODO glpp's error callback is global while on_error is tied to a context. This means that if we create two Contexts glpp will only use the error callback of the second Context.
        on_error(std::move(error_message));
    });
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);                // We use premultiplied alpha, which is the only convention that makes actual sense
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // https://apoorvaj.io/alpha-compositing-opengl-blending-and-premultiplied-alpha/

    glfwSetWindowUserPointer(*_window, this);
    glfwSetWindowSizeCallback(*_window, &window_size_callback);
    glfwSetFramebufferSizeCallback(*_window, &framebuffer_size_callback);
    glfwSetMouseButtonCallback(*_window, &mouse_button_callback);
    glfwSetScrollCallback(*_window, &scroll_callback);
    glfwSetKeyCallback(*_window, &key_callback);
    { // Init _framebuffer_size
        int width, height;
        glfwGetFramebufferSize(*_window, &width, &height);
        on_framebuffer_resize(width, height);
    }

    render_to_screen();
}

void Context::start()
{
    while (!glfwWindowShouldClose(*_window)) {
        if (!glfwGetWindowAttrib(*_window, GLFW_ICONIFIED)) { // Do nothing while the window is minimized. This is here partly because we don't have a proper notion of a window with size 0 and it would currently crash.
            render_to_screen();
            check_for_mouse_movements();
            if (!is_paused()) {
                update();
            }
            _default_canvas.render_target().blit_to(glpp::RenderTarget::screen_framebuffer_id(),
                                                    framebuffer_size(),
                                                    glpp::Interpolation::NearestNeighbour);
            render_to_screen();
            glfwSwapBuffers(*_window);
            _clock->update();
        }
        glfwPollEvents();
    }
    glfwSetWindowShouldClose(*_window, GLFW_FALSE); // Make sure that if start() is called a second time the window won't close instantly the second time
}

/* ------------------------- *
 * ---------DRAWING--------- *
 * ------------------------- */

void Context::background(Color color)
{
    glClearColor(color.r(), color.g(), color.b(), color.a());
    glClear(GL_COLOR_BUFFER_BIT);
}

void Context::square(FullScreen)
{
    square();
}

void Context::square(Center center, Radius radius, Rotation rotation)
{
    rectangle(p6::make_transform_2D(center, radius, rotation));
}

void Context::square(TopLeftCorner corner, Radius radius, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radius, rotation));
}

void Context::square(TopRightCorner corner, Radius radius, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radius, rotation));
}

void Context::square(BottomLeftCorner corner, Radius radius, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radius, rotation));
}

void Context::square(BottomRightCorner corner, Radius radius, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radius, rotation));
}

void Context::rectangle(Center center, Radii radii, Rotation rotation)
{
    rectangle(p6::make_transform_2D(center, radii, rotation));
}

void Context::rectangle(FullScreen)
{
    rectangle(Center{}, Radii{aspect_ratio(), 1.f});
}

void Context::rectangle(TopLeftCorner corner, Radii radii, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle(TopRightCorner corner, Radii radii, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle(BottomLeftCorner corner, Radii radii, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle(BottomRightCorner corner, Radii radii, Rotation rotation)
{
    rectangle(p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle(Transform2D transform)
{
    render_with_rect_shader(transform, false, false);
}

void Context::circle(FullScreen)
{
    circle();
}

void Context::circle(Center center, Radius radius)
{
    ellipse(center, Radii{radius.value, radius.value});
}

void Context::ellipse(FullScreen)
{
    ellipse(Center{}, Radii{aspect_ratio(), 1.f});
}

void Context::ellipse(Center center, Radii radii, Rotation rotation)
{
    ellipse(p6::make_transform_2D(center, radii, rotation));
}

void Context::ellipse(Transform2D transform)
{
    render_with_rect_shader(transform, true, false);
}

static Radii make_radii(RadiusX radiusX, float aspect_ratio)
{
    return {radiusX.value, radiusX.value / aspect_ratio};
}

static Radii make_radii(RadiusY radiusY, float aspect_ratio)
{
    return {radiusY.value * aspect_ratio, radiusY.value};
}

void Context::image(const ImageOrCanvas& img, Center center, RadiusX radiusX, Rotation rotation)
{
    image(img, p6::make_transform_2D(center,
                                     make_radii(radiusX, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, TopLeftCorner corner, RadiusX radiusX, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusX, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, TopRightCorner corner, RadiusX radiusX, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusX, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, BottomLeftCorner corner, RadiusX radiusX, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusX, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, BottomRightCorner corner, RadiusX radiusX, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusX, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, Center center, RadiusY radiusY, Rotation rotation)
{
    image(img, p6::make_transform_2D(center,
                                     make_radii(radiusY, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, TopLeftCorner corner, RadiusY radiusY, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusY, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, TopRightCorner corner, RadiusY radiusY, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusY, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, BottomLeftCorner corner, RadiusY radiusY, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusY, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, BottomRightCorner corner, RadiusY radiusY, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     make_radii(radiusY, img.aspect_ratio()),
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, Center center, Radii radii, Rotation rotation)
{
    image(img, p6::make_transform_2D(center,
                                     radii,
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, FitX)
{
    image(img, Center{},
          RadiusX{aspect_ratio()},
          Rotation{});
}

void Context::image(const ImageOrCanvas& img, FitY)
{
    image(img, Center{},
          RadiusY{},
          Rotation{});
}

void Context::image(const ImageOrCanvas& img, FullScreen)
{
    image(img, make_transform_2D(FullScreen{}));
}

void Context::image(const ImageOrCanvas& img, TopLeftCorner corner, Radii radii, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     radii,
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, TopRightCorner corner, Radii radii, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     radii,
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, BottomLeftCorner corner, Radii radii, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     radii,
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, BottomRightCorner corner, Radii radii, Rotation rotation)
{
    image(img, p6::make_transform_2D(corner,
                                     radii,
                                     rotation));
}

void Context::image(const ImageOrCanvas& img, Transform2D transform)
{
    img.texture().bind_to_texture_unit(0);
    _rect_shader.bind();
    _rect_shader.set("_image", 0);
    render_with_rect_shader(transform, false, true);
}

void Context::set_vertex_shader_uniforms(const Shader& shader, Transform2D transform) const
{
    p6::internal::set_vertex_shader_uniforms(shader, transform, aspect_ratio());
}

template<typename PositionSpecifier>
static void text_impl(Context& ctx, details::TextRenderer& text_renderer, const std::u16string& str,
                      PositionSpecifier position_specifier, Rotation rotation)
{
    text_renderer.setup_rendering_for(str,
                                      {ctx.text_size,
                                       ctx.text_inflating,
                                       ctx.fill});
    ctx.rectangle_with_shader(text_renderer.shader(),
                              position_specifier,
                              details::TextRendererU::compute_text_radii(str, ctx.text_size),
                              rotation);
}

void Context::text(const std::u16string& str, Center center, Rotation rotation)
{
    text_impl(*this, _text_renderer, str, center, rotation);
}

void Context::text(const std::u16string& str, TopLeftCorner corner, Rotation rotation)
{
    text_impl(*this, _text_renderer, str, corner, rotation);
}

void Context::text(const std::u16string& str, TopRightCorner corner, Rotation rotation)
{
    text_impl(*this, _text_renderer, str, corner, rotation);
}

void Context::text(const std::u16string& str, BottomLeftCorner corner, Rotation rotation)
{
    text_impl(*this, _text_renderer, str, corner, rotation);
}

void Context::text(const std::u16string& str, BottomRightCorner corner, Rotation rotation)
{
    text_impl(*this, _text_renderer, str, corner, rotation);
}

void Context::rectangle_with_shader(const Shader& shader, FullScreen)
{
    rectangle_with_shader(shader, make_transform_2D(FullScreen{}));
}

void Context::rectangle_with_shader(const Shader& shader, Center center, Radii radii, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(center, radii, rotation));
}

void Context::rectangle_with_shader(const Shader& shader, TopLeftCorner corner, Radii radii, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle_with_shader(const Shader& shader, TopRightCorner corner, Radii radii, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle_with_shader(const Shader& shader, BottomLeftCorner corner, Radii radii, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radii, rotation));
}

void Context::rectangle_with_shader(const Shader& shader, BottomRightCorner corner, Radii radii, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radii, rotation));
}

void Context::square_with_shader(const Shader& shader, Center center, Radius radius, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(center, radius, rotation));
}

void Context::square_with_shader(const Shader& shader, TopLeftCorner corner, Radius radius, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radius, rotation));
}

void Context::square_with_shader(const Shader& shader, TopRightCorner corner, Radius radius, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radius, rotation));
}

void Context::square_with_shader(const Shader& shader, BottomLeftCorner corner, Radius radius, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radius, rotation));
}

void Context::square_with_shader(const Shader& shader, BottomRightCorner corner, Radius radius, Rotation rotation)
{
    rectangle_with_shader(shader, p6::make_transform_2D(corner, radius, rotation));
}

void Context::rectangle_with_shader(const Shader& shader, Transform2D transform)
{
    set_vertex_shader_uniforms(shader, transform);
    _rect_renderer.render();
}

void Context::line(glm::vec2 start, glm::vec2 end)
{
    _line_shader.set("_material", stroke.as_premultiplied_vec4());
    rectangle_with_shader(_line_shader,
                          Center{(start + end) / 2.f},
                          Radii{glm::distance(start, end) / 2.f, stroke_weight},
                          Rotation{Radians{glm::orientedAngle(glm::vec2{1.f, 0.f}, glm::normalize(end - start))}});
}

void Context::render_with_rect_shader(Transform2D transform, bool is_ellipse, bool is_image) const
{
    _rect_shader.bind();
    set_vertex_shader_uniforms(_rect_shader, transform);
    _rect_shader.set("_is_image", is_image);
    _rect_shader.set("_is_ellipse", is_ellipse);
    _rect_shader.set("_fill_color", use_fill ? fill.as_premultiplied_vec4() : glm::vec4{0.f});
    _rect_shader.set("_stroke_color", stroke.as_premultiplied_vec4());
    _rect_shader.set("_use_stroke", use_stroke);
    _rect_shader.set("_stroke_weight", stroke_weight);
    _rect_renderer.render();
}
/* -------------------------------- *
 * ---------RENDER TARGETS--------- *
 * -------------------------------- */

void Context::render_to_canvas(Canvas& canvas)
{
    canvas.render_target().bind();
}

void Context::render_to_screen()
{
    render_to_canvas(_default_canvas);
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
    return static_cast<float>(framebuffer_width()) / static_cast<float>(framebuffer_height());
}

float Context::inverse_aspect_ratio() const
{
    return static_cast<float>(framebuffer_height()) / static_cast<float>(framebuffer_width());
}

ImageSize Context::framebuffer_size() const
{
    return _framebuffer_size;
}

int Context::framebuffer_width() const
{
    return framebuffer_size().width();
}

int Context::framebuffer_height() const
{
    return framebuffer_size().height();
}

bool Context::window_is_focused() const
{
    return static_cast<bool>(glfwGetWindowAttrib(*_window, GLFW_FOCUSED));
}

void Context::focus_window() const
{
    glfwFocusWindow(*_window);
}

void Context::maximize_window()
{
    glfwMaximizeWindow(*_window);
}

void Context::minimize_window()
{
    glfwIconifyWindow(*_window);
}

void Context::restore_window()
{
    glfwRestoreWindow(*_window);
}

bool Context::window_is_maximized() const
{
    return glfwGetWindowAttrib(*_window, GLFW_MAXIMIZED);
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

void Context::set_time_mode_fixedstep(float framerate)
{
    const auto t          = _clock->time();
    const auto was_paused = !_clock->is_playing();
    _clock                = std::make_unique<details::Clock_FixedTimestep>(framerate);
    _clock->set_time(t);
    if (was_paused) {
        _clock->pause();
    }
}

/* ------------------------------- *
 * ---------MISCELLANEOUS--------- *
 * ------------------------------- */

void Context::stop()
{
    glfwSetWindowShouldClose(*_window, GLFW_TRUE);
}

void Context::pause()
{
    _clock->pause();
}

void Context::resume()
{
    _clock->play();
}

bool Context::is_paused() const
{
    return !_clock->is_playing();
}

/* ------------------------- *
 * ---------PRIVATE--------- *
 * ------------------------- */

glm::vec2 Context::window_to_relative_coords(glm::vec2 pos) const
{
    const auto w = static_cast<float>(window_size().width());
    const auto h = static_cast<float>(window_size().height());

    pos.y = h - pos.y;    // Make y-axis point up
    pos.x -= w / 2.f;     // Center around 0
    pos.y -= h / 2.f;     // Center around 0
    return pos / h * 2.f; // Normalize
}

void Context::on_framebuffer_resize(int width, int height)
{
    if (width > 0 && height > 0) {
        _framebuffer_size = {width, height};
        _default_canvas.resize(_framebuffer_size);
        framebuffer_resized();
    }
}

void Context::on_window_resize(int width, int height)
{
    if (width > 0 && height > 0) {
        _window_size = {width, height};
    }
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

void Context::on_key(int key_code, int scancode, int action, int /*mods*/)
{
    const char* key_name = glfwGetKeyName(key_code, scancode);
    const auto  key      = Key{key_code == GLFW_KEY_SPACE ? " " : key_name ? key_name
                                                                           : "",
                         key_code};
    if (action == GLFW_PRESS) {
        key_pressed(key);
    }
    else if (action == GLFW_REPEAT) {
        key_repeated(key);
    }
    else if (action == GLFW_RELEASE) {
        key_released(key);
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

Transform2D Context::make_transform_2D(FullScreen) const
{
    return p6::make_transform_2D(Center{},
                                 Radii{aspect_ratio(), 1.f},
                                 Rotation{});
}

} // namespace p6