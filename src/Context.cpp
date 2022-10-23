#include "Context.h"
#include <cstdint>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <img/img.hpp>
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
    , _window_width_before_fullscreen{window_creation_params.width}
    , _window_height_before_fullscreen{window_creation_params.height}
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

    internal::ImGuiWrapper::initialize(*_window); // Must be after all the glfwSetXxxCallback, otherwise they will override the ImGui callbacks

    render_to_main_canvas();
    framerate_synced_with_monitor();
}

static bool needs_to_wait_to_cap_framerate(std::optional<std::chrono::nanoseconds> capped_delta_time,
                                           std::chrono::steady_clock::time_point   last_update)
{
    if (!capped_delta_time)
    {
        return false;
    }
    const auto delta_time = std::chrono::steady_clock::now() - last_update;
    return delta_time < *capped_delta_time;
}

static bool skip_first_frames(internal::Clock& clock)
{
    static int frame_count = 0;
    if (frame_count < 2)
    {
        frame_count++;
        clock.update();
        return true;
    }
    else
    {
        return false;
    }
}

static auto to_img_size(const ImageSize& size)
{
    return img::SizeT<GLsizei>{size.width(),
                               size.height()};
}

static auto to_p6_size(const img::SizeT<float>& size)
{
    return ImageSize{static_cast<GLsizei>(size.width()),
                     static_cast<GLsizei>(size.height())};
}

ImageSize Context::main_canvas_displayed_size_inside_window()
{
    return to_p6_size(img::SizeU::fit_into(to_img_size(_framebuffer_size),
                                           to_img_size(_main_canvas.size())));
}

void Context::start()
{
    while (!glfwWindowShouldClose(*_window))
    {
        if (!glfwGetWindowAttrib(*_window, GLFW_ICONIFIED)) // Do nothing while the window is minimized. This is here partly because we don't have a proper notion of a window with size 0 and it would currently crash.
        {
            if (!skip_first_frames(*_clock)) // Allow the clock to compute its delta_time() properly
            {
                internal::ImGuiWrapper::begin_frame();
                // Clear the window in case the default canvas doesn't cover the whole window
                glpp::bind_framebuffer(glpp::SCREEN_FRAMEBUFFER_ID);
                glClearColor(0.3f, 0.3f, 0.3f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT);

                render_to_main_canvas();
                check_for_mouse_movements();

                if (!is_paused()
                    && !needs_to_wait_to_cap_framerate(_capped_delta_time, _last_update))
                {
                    _clock->update();
                    _last_update = std::chrono::steady_clock::now();
                    update();
                }

                const auto size_inside_window = main_canvas_displayed_size_inside_window();
                const auto pos_inside_window  = glpp::BlitTopLeftCorner{_framebuffer_size.width() / 2 - size_inside_window.width() / 2,
                                                                       _framebuffer_size.height() / 2 - size_inside_window.height() / 2};
                _main_canvas.render_target().blit_to(glpp::RenderTarget::screen_framebuffer_id(),
                                                     size_inside_window,
                                                     glpp::Interpolation::NearestNeighbour,
                                                     pos_inside_window);
                glpp::bind_framebuffer(glpp::RenderTarget::screen_framebuffer_id());
                imgui();
                internal::ImGuiWrapper::end_frame(*_window);
            }
            glfwSwapBuffers(*_window);
            render_to_main_canvas();
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

void Context::triangle(Point2D p1, Point2D p2, Point2D p3)
{
    _triangle_renderer.render(p1.value, p2.value, p3.value,
                              static_cast<float>(current_canvas_height()), aspect_ratio(),
                              use_fill ? std::make_optional(fill.as_premultiplied_vec4()) : std::nullopt,
                              use_stroke ? std::make_optional(stroke.as_premultiplied_vec4()) : std::nullopt,
                              stroke_weight);
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

void Context::image(const ImageOrCanvas& img, Fit)
{
    if (img.aspect_ratio() < aspect_ratio())
    {
        image(img, FitY{});
    }
    else
    {
        image(img, FitX{});
    }
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
static void text_impl(Context& ctx, internal::TextRenderer& text_renderer, const std::u16string& str,
                      PositionSpecifier position_specifier, Rotation rotation)
{
    text_renderer.setup_rendering_for(str, ctx.fill, ctx.text_inflating);
    ctx.rectangle_with_shader(text_renderer.shader(),
                              position_specifier,
                              internal::TextRendererU::compute_text_radii(str, ctx.text_size),
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
                          Radii{glm::distance(start, end) / 2.f + stroke_weight, stroke_weight},
                          Rotation{Radians{glm::orientedAngle(glm::vec2{1.f, 0.f},
                                                              glm::normalize(end - start))}});
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
    _current_canvas = canvas;
}

void Context::render_to_main_canvas()
{
    render_to_canvas(_main_canvas);
}

void Context::main_canvas_mode(CanvasSizeMode mode)
{
    _main_canvas_size_mode = mode;
    adapt_main_canvas_size_to_framebuffer_size();
}

static float canvas_ratio_impl(float canvas_ratio, float frame_ratio)
{
    if (canvas_ratio > frame_ratio)
    {
        return canvas_ratio / frame_ratio;
    }
    else
    {
        return 1.f;
    }
}

float Context::canvas_ratio(const Canvas& canvas) const
{
    return canvas_ratio_impl(canvas.aspect_ratio(),
                             _main_canvas.aspect_ratio());
}

/* ----------------------- *
 * ---------INPUT--------- *
 * ----------------------- */

glm::vec2 Context::mouse() const
{
    return _mouse_position;
}

glm::vec2 Context::mouse_delta() const
{
    return _mouse_position_delta;
}

bool Context::mouse_is_in_window() const
{
    if (!window_is_focused())
    {
        return false;
    }
    const auto pos = mouse();
    return pos.x >= -aspect_ratio() && pos.x <= aspect_ratio() && pos.y >= -1.f && pos.y <= 1.f;
}

bool Context::ctrl() const
{
    return glfwGetKey(*_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(*_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
}

bool Context::shift() const
{
    return glfwGetKey(*_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(*_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

bool Context::alt() const
{
    return glfwGetKey(*_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(*_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
}

/* ------------------------ *
 * ---------WINDOW--------- *
 * ------------------------ */

float Context::aspect_ratio() const
{
    return current_canvas().aspect_ratio();
}

float Context::inverse_aspect_ratio() const
{
    return current_canvas().inverse_aspect_ratio();
}

ImageSize Context::main_canvas_size() const
{
    return _main_canvas.size();
}

int Context::main_canvas_width() const
{
    return main_canvas_size().width();
}

int Context::main_canvas_height() const
{
    return main_canvas_size().height();
}

ImageSize Context::current_canvas_size() const
{
    return current_canvas().size();
}

int Context::current_canvas_width() const
{
    return current_canvas_size().width();
}

int Context::current_canvas_height() const
{
    return current_canvas_size().height();
}

Color Context::read_pixel(glm::vec2 position) const
{
    const auto x = static_cast<int>(p6::map(position.x,
                                            -main_canvas_size().aspect_ratio(), +main_canvas_size().aspect_ratio(),
                                            0.f, static_cast<float>(main_canvas_width())));
    const auto y = static_cast<int>(p6::map(position.y,
                                            -1.f, +1.f,
                                            0.f, static_cast<float>(main_canvas_height())));
    uint8_t    channels[4];
    GLint      previous_framebuffer;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previous_framebuffer);
    glpp::bind_framebuffer_as_read(main_canvas().render_target().framebuffer());
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, channels);
    glpp::bind_framebuffer_as_read(static_cast<GLuint>(previous_framebuffer));
    return p6::Color{static_cast<float>(channels[0]) / 255.f,
                     static_cast<float>(channels[1]) / 255.f,
                     static_cast<float>(channels[2]) / 255.f,
                     static_cast<float>(channels[3]) / 255.f};
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

static GLFWmonitor* current_monitor(GLFWwindow* window)
{
    // Thanks to https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
    int                nmonitors, i;
    int                wx, wy, ww, wh;
    int                mx, my, mw, mh;
    int                overlap, bestoverlap;
    GLFWmonitor*       bestmonitor;
    GLFWmonitor**      monitors;
    const GLFWvidmode* mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++)
    {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap = std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) * std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

        if (bestoverlap < overlap)
        {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}

void Context::go_fullscreen()
{
    if (!window_is_fullscreen())
    {
        glfwGetWindowPos(*_window, &_window_pos_x_before_fullscreen, &_window_pos_y_before_fullscreen);
        glfwGetWindowSize(*_window, &_window_width_before_fullscreen, &_window_height_before_fullscreen);

        GLFWmonitor*       monitor = current_monitor(*_window);
        const GLFWvidmode* mode    = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(*_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        _window_is_fullscreen = true;
    }
}

void Context::escape_fullscreen()
{
    if (window_is_fullscreen())
    {
        glfwSetWindowMonitor(*_window, nullptr,
                             _window_pos_x_before_fullscreen,
                             _window_pos_y_before_fullscreen,
                             _window_width_before_fullscreen,
                             _window_height_before_fullscreen,
                             0);
        _window_is_fullscreen = false;
    }
}

void Context::toggle_fullscreen()
{
    if (window_is_fullscreen())
    {
        escape_fullscreen();
    }
    else
    {
        go_fullscreen();
    }
}

bool Context::window_is_fullscreen() const
{
    return _window_is_fullscreen;
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

void Context::time_perceived_as_realtime()
{
    const auto t          = _clock->time();
    const auto was_paused = !_clock->is_playing();
    _clock                = std::make_unique<internal::Clock_Realtime>();
    _clock->set_time(t);
    if (was_paused)
    {
        _clock->pause();
    }
}

void Context::time_perceived_as_constant_delta_time(float framerate)
{
    const auto t          = _clock->time();
    const auto was_paused = !_clock->is_playing();
    _clock                = std::make_unique<internal::Clock_FixedTimestep>(framerate);
    _clock->set_time(t);
    if (was_paused)
    {
        _clock->pause();
    }
}

void Context::framerate_synced_with_monitor()
{
    glfwSwapInterval(1);
    _capped_delta_time.reset();
}

void Context::framerate_as_high_as_possible()
{
    glfwSwapInterval(0);
    _capped_delta_time.reset();
}

void Context::framerate_capped_at(float framerate)
{
    glfwSwapInterval(0);
    _capped_delta_time = std::chrono::nanoseconds{static_cast<std::chrono::nanoseconds::rep>(
        1000000000.f / framerate // Convert from fps to nanoseconds
        )};
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

float Context::default_canvas_ratio() const
{
    return canvas_ratio_impl(_main_canvas.aspect_ratio(),
                             _window_size.aspect_ratio());
}

glm::vec2 Context::window_to_relative_coords(glm::vec2 pos) const
{
    const auto w = static_cast<float>(_window_size.width());
    const auto h = static_cast<float>(_window_size.height());

    pos.y = h - pos.y;             // Make y-axis point up
    pos.x -= w / 2.f;              // Center around 0
    pos.y -= h / 2.f;              // Center around 0
    pos /= h / 2.f;                // Normalize
    pos *= default_canvas_ratio(); // Adapt to the canvas
    return pos;
}

namespace internal {

static void adapt_canvas_size_to_framebuffer_size(Canvas& canvas, ImageSize size,
                                                  CanvasSizeMode_SameAsWindow)
{
    canvas.resize(size);
}

static void adapt_canvas_size_to_framebuffer_size(Canvas&                  canvas, ImageSize,
                                                  CanvasSizeMode_FixedSize mode)
{
    canvas.resize(mode.size);
}

static void adapt_canvas_size_to_framebuffer_size(Canvas& canvas, ImageSize size,
                                                  CanvasSizeMode_FixedAspectRatio mode)
{
    canvas.resize(to_p6_size(img::SizeU::fit_into(to_img_size(size), mode.aspect_ratio)));
}

static void adapt_canvas_size_to_framebuffer_size(Canvas& canvas, ImageSize size,
                                                  CanvasSizeMode_RelativeToWindow mode)
{
    canvas.resize({static_cast<GLsizei>(mode.width_scale * static_cast<float>(size.width())),
                   static_cast<GLsizei>(mode.height_scale * static_cast<float>(size.height()))});
}

} // namespace internal

void Context::adapt_main_canvas_size_to_framebuffer_size()
{
    std::visit([&](auto&& mode) { internal::adapt_canvas_size_to_framebuffer_size(_main_canvas, _framebuffer_size, mode); }, _main_canvas_size_mode);
    main_canvas_resized();
}

void Context::on_framebuffer_resize(int width, int height)
{
    if (width > 0 && height > 0)
    {
        _framebuffer_size = {width, height};
        adapt_main_canvas_size_to_framebuffer_size();
    }
}

void Context::on_window_resize(int width, int height)
{
    if (width > 0 && height > 0)
    {
        _window_size = {width, height};
    }
}

void Context::on_mouse_button(int button, int action, int /*mods*/)
{
    const auto mouse_button = [&]() {
        switch (button)
        {
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
    if (action == GLFW_PRESS)
    {
        _is_dragging         = true;
        _drag_start_position = _mouse_position;
        mouse_pressed(button_event);
    }
    else if (action == GLFW_RELEASE)
    {
        _is_dragging = false;
        mouse_released(button_event);
    }
    else
    {
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
    if (action == GLFW_PRESS)
    {
        if (key.physical == GLFW_KEY_F11)
        {
            toggle_fullscreen();
        }
        if (key.physical == GLFW_KEY_ESCAPE)
        {
            escape_fullscreen();
        }
        key_pressed(key);
    }
    else if (action == GLFW_REPEAT)
    {
        key_repeated(key);
    }
    else if (action == GLFW_RELEASE)
    {
        key_released(key);
    }
    else
    {
        throw std::runtime_error("[p6 internal error] Unknown key action: " + std::to_string(action));
    }
}

void Context::on_mouse_move()
{
    if (_is_dragging)
    {
        mouse_dragged({mouse(), mouse_delta(), _drag_start_position});
    }
    else
    {
        mouse_moved({mouse(), mouse_delta()});
    }
}

void Context::check_for_mouse_movements()
{
    const auto mouse_pos = compute_mouse_position();
    if (mouse_pos != _mouse_position)
    {
        _mouse_position_delta = mouse_pos - _mouse_position;
        _mouse_position       = mouse_pos;
        if (window_is_focused())
        {
            on_mouse_move();
        }
    }
    else
    {
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
