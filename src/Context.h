#pragma once

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <functional>
#include <glm/glm.hpp>
#include <glpp/extended.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include "Canvas.h"
#include "Color.h"
#include "Event.h"
#include "Image.h"
#include "Shader.h"
#include "Transform2D.h"
#include "internal/ImGuiWrapper.h"
#include "internal/RectRenderer.h"
#include "internal/TextRenderer.h"
#include "internal/Time/Clock.h"
#include "internal/Time/Clock_FixedTimestep.h"
#include "internal/Time/Clock_Realtime.h"
#include "internal/TransformStack.h"
#include "internal/TriangleRenderer.h"
#include "internal/UniqueGlfwWindow.h"

namespace p6 {

struct FullScreen {};
struct Fit {};
struct FitX {};
struct FitY {};

struct Point2D {
    glm::vec2 value;

    Point2D(float x, float y)
        : value{x, y} {}

    Point2D(glm::vec2 value)
        : value{value} {}
};

/// The canvas will have the same size as the window's framebuffer
struct CanvasSizeMode_SameAsWindow {};

/// The canvas will have a size that perfectly fits into the window's framebuffer, while keeping the given aspect ratio
struct CanvasSizeMode_FixedAspectRatio {
    float aspect_ratio;
};

/// The canvas will have a given size, independently of the size of the window's framebuffer
struct CanvasSizeMode_FixedSize {
    ImageSize size;
};

/// The canvas will have a size that is a multiple of the windows's framebuffer size
struct CanvasSizeMode_RelativeToWindow {
    explicit CanvasSizeMode_RelativeToWindow(float scale)
        : width_scale{scale}
        , height_scale{scale}
    {
    }

    CanvasSizeMode_RelativeToWindow(float width_scale, float height_scale)
        : width_scale{width_scale}
        , height_scale{height_scale}
    {
    }

    float width_scale;
    float height_scale;
};

using CanvasSizeMode = std::variant<CanvasSizeMode_SameAsWindow,
                                    CanvasSizeMode_FixedAspectRatio,
                                    CanvasSizeMode_FixedSize,
                                    CanvasSizeMode_RelativeToWindow>;

class Context;
class [[nodiscard("You need to assign this to a variable. It will push_transform() when created and pop_transform() when it goes out of scope. See liendoc")]] TransformScopeGuard // NOLINT(*-special-member-functions)
{
public:
    explicit TransformScopeGuard(Context & ctx);
    ~TransformScopeGuard();

private:
    Context& _ctx;
};

class Context {
public:
    Context(WindowCreationParams window_creation_params = {});

    Context(Context&&) noexcept            = default;
    Context& operator=(Context&&) noexcept = default;

    Context(const Context&)            = delete;
    Context& operator=(const Context&) = delete;

    /* ------------------------------- */
    /** \defgroup events Events
     * You can set those functions as you wish to react to various events.
     * The simplest way is to use a lambda:
     *
     * ```
     * auto ctx = p6::Context{};
     * ctx.mouse_pressed = [](p6::MouseButton) {
     *     std::cout << "Hello World\n";
     * };
     * ```
     * @{*/
    /* ------------------------------- */

    /// This function is called repeatedly. The framerate is controlled by the framerate_xxx() functions.
    std::function<void()> update = []() {
    };
    /// In this function you can call all the ImGui code you want.
    /// Note that you can also do it during the `update()` function.
    /// This `imgui()` function is just here to help you organize your code if you want to.
    std::function<void()> imgui = []() {
    };
    /// This function is called whenever the mouse is moved
    std::function<void(MouseMove)> mouse_moved = [](MouseMove) {
    };
    /// This function is called whenever the mouse is dragged
    std::function<void(MouseDrag)> mouse_dragged = [](MouseDrag) {
    };
    /// This function is called whenever a mouse button is pressed
    std::function<void(MouseButton)> mouse_pressed = [](MouseButton) {
    };
    /// This function is called whenever a mouse button is released
    std::function<void(MouseButton)> mouse_released = [](MouseButton) {
    };
    /// This function is called whenever the mouse wheel is scrolled
    std::function<void(MouseScroll)> mouse_scrolled = [](MouseScroll) {
    };
    /// This function is called whenever a keyboard key is pressed
    std::function<void(Key)> key_pressed = [](Key) {
    };
    /// This function is called whenever a keyboard key is released
    std::function<void(Key)> key_released = [](Key) {
    };
    /// This function is called repeatedly whenever a keyboard key is held. (NB: this only starts after holding the key for a little while. The exact behaviour is OS-specific)
    ///
    /// :warning: This is less than ideal to do things like handling the movement of a character. You should rather do, in your update function:
    ///
    /// ```
    /// if (ctx.key_is_held(GLFW_KEY_W)) {
    ///     character.move_forward(ctx.delta_time());
    /// }
    /// ```
    std::function<void(Key)> key_repeated = [](Key) {
    };
    /// This function is called whenever an error occurs.
    std::function<void(std::string&&)> on_error = [](std::string&& error_message) {
        std::cerr << error_message << '\n';
        throw std::runtime_error{error_message};
    };
    /// This function is called whenever the main canvas is resized.
    /// If you call main_canvas_size(), main_canvas_width(), main_canvas_height() or aspect_ratio() inside main_canvas_resized() they will already be referring to the new size.
    std::function<void()> main_canvas_resized = []() {
    };
    /// This function is called whenever any event occurs (key pressed, mouse moved, etc.).
    /// It can be useful to use this function instead of the more specific ones (key_pressed, mouse_moved, etc.) if for example you want
    /// to forward several events to a function that will handle them. For example in order to control a camera you might need to forward the key, mouse and update events to it.
    /// Instead of having to put the code inside those three event functions, you can just put it in `on_event` and let the camera handle each event as it so pleases.
    std::function<void(Event)> on_event = [](Event const&) {
    };

    /**@}*/
    /* ------------------------------- */
    /** \defgroup drawing Drawing
     * Draw shapes and control the look of them.
     * @{*/
    /* ------------------------------- */

    /// The color that is used for the interior of the shapes.
    Color fill{1.f, 1.f, 1.f, 0.5f};
    /// Whether the shapes will have an interior
    bool use_fill = true;
    /// The color that is used for the boundary of the shapes.
    Color stroke{0.f, 0.f, 0.f};
    /// The size of the boundary of the shapes.
    float stroke_weight = 0.01f;
    /// Whether there will be a boundary on the shape.
    bool use_stroke = true;

    /// Sets the color and alpha of each pixel of the canvas.
    /// NB: No blending is applied; even if you specify an alpha of 0.5 the old canvas is completely erased. This means that setting an alpha here doesn't matter much. It is only meaningful if you export the canvas as a png, or if you later try to blend the canvas on top of another image.
    void background(Color color);

    /// Draws a square
    void square(FullScreen);
    void square(Center = {}, Radius = {}, Rotation = {});
    void square(TopLeftCorner, Radius = {}, Rotation = {});
    void square(TopRightCorner, Radius = {}, Rotation = {});
    void square(BottomLeftCorner, Radius = {}, Rotation = {});
    void square(BottomRightCorner, Radius = {}, Rotation = {});

    /// Draws a rectangle
    void rectangle(FullScreen = {});
    void rectangle(Center, Radii = {}, Rotation = {});
    void rectangle(TopLeftCorner, Radii = {}, Rotation = {});
    void rectangle(TopRightCorner, Radii = {}, Rotation = {});
    void rectangle(BottomLeftCorner, Radii = {}, Rotation = {});
    void rectangle(BottomRightCorner, Radii = {}, Rotation = {});
    void rectangle(Transform2D);

    /// Draws a circle
    void circle(FullScreen);
    void circle(Center = {}, Radius = {});

    /// Draws an ellipse
    void ellipse(FullScreen = {});
    void ellipse(Center, Radii = {}, Rotation = {});
    void ellipse(Transform2D);

    /// Draws an equilateral triangle. `Radius` is the distance between the `Center` and one of the tips of the triangle.
    void equilateral_triangle(Center, Radius = {}, Rotation = {});
    /// Draws a triangle between the three points, translated by `Center` and rotated by `Rotation`.
    void triangle(Point2D, Point2D, Point2D, Center = {}, Rotation = {});
    /// Draws a triangle between the three points, and applies the transform to the triangle.
    void triangle(Point2D, Point2D, Point2D, Transform2D);

    /// Draws an image as big as possible on the screen. This will respect the aspect ratio of the image.
    void image(const ImageOrCanvas&, Fit = {});
    void image(const ImageOrCanvas&, FitX);
    void image(const ImageOrCanvas&, FitY);
    /// Draws an image. This will respect the aspect ratio of the image.
    void image(const ImageOrCanvas&, Center, RadiusX = {}, Rotation = {});
    void image(const ImageOrCanvas&, TopLeftCorner, RadiusX = {}, Rotation = {});
    void image(const ImageOrCanvas&, TopRightCorner, RadiusX = {}, Rotation = {});
    void image(const ImageOrCanvas&, BottomLeftCorner, RadiusX = {}, Rotation = {});
    void image(const ImageOrCanvas&, BottomRightCorner, RadiusX = {}, Rotation = {});
    /// Draws an image. This will respect the aspect ratio of the image.
    void image(const ImageOrCanvas&, Center, RadiusY = {}, Rotation = {});
    void image(const ImageOrCanvas&, TopLeftCorner, RadiusY = {}, Rotation = {});
    void image(const ImageOrCanvas&, TopRightCorner, RadiusY = {}, Rotation = {});
    void image(const ImageOrCanvas&, BottomLeftCorner, RadiusY = {}, Rotation = {});
    void image(const ImageOrCanvas&, BottomRightCorner, RadiusY = {}, Rotation = {});
    /// Draws an image that takes the entire window. :warning: This might distort the image if the window doesn't have the same aspect ratio as the image.
    void image(const ImageOrCanvas&, FullScreen);
    /// Draws an image. :warning: This might distort the image if radii doesn't have the same aspect ratio as the image.
    void image(const ImageOrCanvas&, Center, Radii = {}, Rotation = {});
    void image(const ImageOrCanvas&, TopLeftCorner, Radii = {}, Rotation = {});
    void image(const ImageOrCanvas&, TopRightCorner, Radii = {}, Rotation = {});
    void image(const ImageOrCanvas&, BottomLeftCorner, Radii = {}, Rotation = {});
    void image(const ImageOrCanvas&, BottomRightCorner, Radii = {}, Rotation = {});
    void image(const ImageOrCanvas&, Transform2D);

    /// Draws a rectangle using a custom fragment shader
    void rectangle_with_shader(const Shader& shader, FullScreen = {});
    void rectangle_with_shader(const Shader& shader, Center, Radii = {}, Rotation = {});
    void rectangle_with_shader(const Shader& shader, TopLeftCorner, Radii = {}, Rotation = {});
    void rectangle_with_shader(const Shader& shader, TopRightCorner, Radii = {}, Rotation = {});
    void rectangle_with_shader(const Shader& shader, BottomLeftCorner, Radii = {}, Rotation = {});
    void rectangle_with_shader(const Shader& shader, BottomRightCorner, Radii = {}, Rotation = {});
    void square_with_shader(const Shader& shader, Center = {}, Radius = {}, Rotation = {});
    void square_with_shader(const Shader& shader, TopLeftCorner, Radius = {}, Rotation = {});
    void square_with_shader(const Shader& shader, TopRightCorner, Radius = {}, Rotation = {});
    void square_with_shader(const Shader& shader, BottomLeftCorner, Radius = {}, Rotation = {});
    void square_with_shader(const Shader& shader, BottomRightCorner, Radius = {}, Rotation = {});
    void rectangle_with_shader(const Shader& shader, Transform2D);

    /// Draws a line between two points.
    /// It uses the `stroke` color, and `stroke_weight` as its thickness.
    void line(glm::vec2 start, glm::vec2 end);

    /**@}*/
    /* ------------------------------- */
    /** \defgroup text Text
     * Write text to the screen.
     * @{*/
    /* ------------------------------- */

    /// Height of the text.
    float text_size = 0.03f;
    /// Gives some "boldness" to the text.
    float text_inflating = 0.01f;

    void text(const std::u16string& str, Center, Rotation = {});
    void text(const std::u16string& str, TopLeftCorner, Rotation = {});
    void text(const std::u16string& str, TopRightCorner, Rotation = {});
    void text(const std::u16string& str, BottomLeftCorner, Rotation = {});
    void text(const std::u16string& str, BottomRightCorner, Rotation = {});

    /**@}*/
    /* ------------------------------- */
    /** \defgroup transform Transform
     * Allows you to translate / rotate / scale all your drawings at once.
     * @{*/
    /* ------------------------------- */

    /// Applies a translation to all the future drawings. To undo this, see `push_transform()` and `pop_transform()`.
    void translate(glm::vec2 translation) { _transform_stack.translate(translation); }
    /// Applies a rotation to all the future drawings. To undo this, see `push_transform()` and `pop_transform()`.
    void rotate(p6::Angle angle) { _transform_stack.rotate(angle); }
    /// Applies a scale to all the future drawings. To undo this, see `push_transform()` and `pop_transform()`.
    void scale(glm::vec2 scale_factor) { _transform_stack.scale(scale_factor); }
    /// Undoes all the translate / rotate / scale / apply_transform / set_transform.
    void reset_transform() { _transform_stack.reset_matrix(); }
    /// Applies a custom transform matrix to all the future drawings. To undo this, see `push_transform()` and `pop_transform()`.
    void apply_transform(glm::mat3 additional_transform) { _transform_stack.apply_matrix(additional_transform); }
    /// Sets the current transform matrix. This will override all the previous translate / rotate / scale / apply_transform. To undo this, see `push_transform()` and `pop_transform()`.
    void set_transform(glm::mat3 transform) { _transform_stack.set_matrix(transform); }

    /// Saves the current transform state.
    void push_transform() { _transform_stack.push_transform(); }
    /// Restores the transform to the state it had during the last `push_transform()` that has not been popped yet.
    void pop_transform() { _transform_stack.pop_transform(); }
    TransformScopeGuard transform_scope_guard() { return TransformScopeGuard{*this}; }

    /// Returns the current transform matrix that is the combination of all the translate / rotate / scale / apply_transform / set_transform that have been applied.
    glm::mat3 current_transform() const { return _transform_stack.current_matrix(); }

    /**@}*/
    /* ------------------------------- */
    /** \defgroup canvas Canvas
     * You can either draw directly in the window (the default) or onto a custom Canvas.
     * @{*/
    /* ------------------------------- */

    /// Sets a canvas to be the one that all the drawing commands will draw on, until you call render_to_main_canvas.
    void render_to_canvas(Canvas&);
    /// Reset the Context to render to the main canvas. The main canvas is the one that will be displayed in the window.
    void render_to_main_canvas();

    Canvas&       main_canvas() { return _main_canvas; }
    const Canvas& main_canvas() const { return _main_canvas; }
    Canvas&       current_canvas() { return _current_canvas; }
    const Canvas& current_canvas() const { return _current_canvas; }

    /// Returns the aspect ratio (a.k.a. width / height) of the current canvas.
    /// This canvas is the window's main canvas by default, unless you called render_to_canvas() in which case it will be the given canvas.
    /// When you call render_to_main_canvas() the current canvas goes back to beeing the window's main canvas.
    float aspect_ratio() const;

    /// Returns the inverse aspect ratio (a.k.a. height / width) of the current canvas.
    /// This canvas is the window's main canvas by default, unless you called render_to_canvas() in which case it will be the given canvas.
    /// When you call render_to_main_canvas() the current canvas goes back to beeing the window's main canvas.
    float inverse_aspect_ratio() const;

    /// Returns the size of the main canvas (width and height).
    ImageSize main_canvas_size() const;

    /// Returns the width of the main canvas.
    int main_canvas_width() const;

    /// Returns the height of the main canvas.
    int main_canvas_height() const;

    /// Returns the size of the current canvas (width and height).
    ImageSize current_canvas_size() const;

    /// Returns the width of the current canvas.
    int current_canvas_width() const;

    /// Returns the height of the current canvas.
    int current_canvas_height() const;

    /// Sets how the size of the main canvas will be computed.
    /// The default mode is CanvasSizeMode_SameAsWindow.
    void main_canvas_mode(CanvasSizeMode);

    /// Returns the ratio that you need to multiply with in order to match the position and size of the `canvas` when it is fitted in the window.
    float canvas_ratio(const Canvas& canvas) const;

    /// Saves the content of the window's main canvas as an image file.
    /// Supported file types are .png and .jpeg/.jpg
    /// Simply use the corresponding extension to save in the desired format.
    /// If the path already exists, a number will be appended to the name and the previous file won't be overwritten.
    /// If the path is relative, it will be relative to the directory containing your executable.
    /// If some directories in the path don't exist yet, they will be created automatically.
    void save_image(std::filesystem::path path) const
    {
        p6::save_image(_main_canvas, path);
    }

    /// Returns the color of the pixel at the given position in the main canvas.
    /// The coordinates are expressed in the usual p6 coordinate system.
    Color read_pixel(glm::vec2 position) const;

    /**@}*/
    /* ------------------------------- */
    /** \defgroup input Input
     * Query the state of the mouse and keyboard.
     * @{*/
    /* ------------------------------- */

    /// Returns the current mouse position
    glm::vec2 mouse() const;
    /// Returns the movement of the mouse since last update().
    glm::vec2 mouse_delta() const;
    /// Returns true iff the window is focused and the coordinates returned by mouse() correspond to a position inside the window.
    bool mouse_is_in_window() const;
    /// Returns true iff the given mouse button is currently pressed.
    bool mouse_button_is_pressed(Button) const;

    /// Returns true iff the CTRL key is pressed (or CMD on Mac)
    bool ctrl() const;
    /// Returns true iff the SHIFT key is pressed
    bool shift() const;
    /// Returns true iff the ALT key is pressed
    bool alt() const;
    /// Returns true iff the given `key` is currently pressed.
    /// `key` should be a GLFW_KEY_ value. See https://www.glfw.org/docs/3.3/group__keys.html for the complete list.
    /// e.g. `ctx.key_is_pressed(GLFW_KEY_Q)`
    /// /!\\ Please note that these are physical keys that don't depend on the current layout.
    /// This means that GLFW_KEY_Q will refer to the first key on your keyboard, no matter if you are using QWERTY or AZERTY.
    bool key_is_pressed(int key) const;

    /**@}*/
    /* ------------------------------- */
    /** \defgroup window Window
     * Query the state of the window and control it.
     * @{*/
    /* ------------------------------- */

    /// Returns true iff the window is currently focused.
    bool window_is_focused() const;
    /// Focuses the window, making it pop to the foreground.
    void focus_window() const;
    /// Maximizes the window.
    void maximize_window();
    /// Minimizes the window. Note that while your window is minimized everything will be frozen and no update, event or anything will happen.
    void minimize_window();
    /// Restores the window if it is currently maximized. Does nothing otherwise.
    void restore_window();
    /// Returns true iff the window is currently maximized.
    bool window_is_maximized() const;
    /// Makes the window fullscreen.
    /// Does nothing if it was already fullscreen.
    void go_fullscreen();
    /// Exits the fullscreen mode.
    /// Does nothing if the window wasn't fullscreen.
    void escape_fullscreen();
    /// Goes fullscreen if it wasn't, escapes fullscreen if it was.
    void toggle_fullscreen();
    /// Returns true iff the window is currently fullscreen.
    bool window_is_fullscreen() const;

    /**@}*/
    /* ------------------------------- */
    /** \defgroup time Time
     * Query time information and control how it elapses.
     * @{*/
    /* ------------------------------- */

    /// Returns the time in seconds since the creation of the Context.
    float time() const;

    /// Returns an estimate of the time that elapses between two update() calls.
    float delta_time() const;

    /// Sets the time mode as *realtime*.
    /// This means that what is returned by time() and delta_time() corresponds to the actual time that elapsed in the real world.
    /// This is ideal when you want to do realtime animation and interactive sketches.
    void time_perceived_as_realtime();

    /// Sets the time mode as *constant delta time*.
    /// This means that what is returned by time() and delta_time() corresponds to an ideal world where there is exactly `1/framerate` seconds between each updates.
    /// This is ideal when you are exporting a video and don't want the long export time to influence your animation.
    /// `framerate` is expressed in frames per second
    void time_perceived_as_constant_delta_time(float framerate);

    /// Makes sure that the framerate is adapted to your monitor: it will be 60 fps if you have a 60 Hertz monitor (which is the most common), or 120 fps if you have a 120 Hertz monitor, etc.
    /// This is the default framerate mode.
    void framerate_synced_with_monitor();

    /// Removes any limit on the framerate. update() will be called as fast as possible.
    void framerate_as_high_as_possible();

    /// Keeps the framerate at the given value.
    void framerate_capped_at(float framerate);

    /**@}*/
    /* ------------------------------- */
    /** \defgroup update-flow Update Flow
     * Control the update() loop and query information about it.
     * @{*/
    /* ------------------------------- */

    /// Starts the update() loop.
    /// update() will be called repeatedly, until you close the window or call stop().
    void start();

    /// Stops the update() loop.
    /// This is the programatic equivalent of a user closing the window.
    void stop();

    /// Pauses the update() loop.
    /// No update() will be called, until you call resume(). User inputs are still processed.
    void pause();

    /// Resumes the update() loop if it was paused with pause().
    /// It has no effect if the loop was already playing.
    void resume();

    /// Returns true iff the update() loop is currently paused.
    bool is_paused() const;

    /**@}*/
private:
    glm::vec2 window_to_relative_coords(glm::vec2 pos) const;
    float     default_canvas_ratio() const;

    void        on_framebuffer_resize(int width, int height);
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void        on_window_resize(int width, int height);
    friend void window_size_callback(GLFWwindow* window, int width, int height);
    void        on_mouse_scroll(double x, double y);
    friend void scroll_callback(GLFWwindow* window, double x, double y);
    void        on_mouse_button(int button, int action, int mods);
    friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void        on_key(int key, int scancode, int action, int mods);
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void      on_mouse_move();
    void      check_for_mouse_movements();
    glm::vec2 compute_mouse_position() const;

    void adapt_main_canvas_size_to_framebuffer_size();

    ImageSize main_canvas_displayed_size_inside_window();

    glm::mat3 complete_transform_matrix(const Transform2D&) const;

    void set_vertex_shader_uniforms(const Shader& shader, Transform2D transform) const;
    void render_with_rect_shader(Transform2D transform, bool is_ellipse, bool is_image) const;

    Transform2D make_transform_2D_impl(glm::vec2 offset_to_center, glm::vec2 corner_position, Radii radii, Rotation rotation) const;
    Transform2D make_transform_2D(FullScreen) const;

private:
    internal::ImGuiWrapper::Raii            _imgui_raii;
    mutable internal::UniqueGlfwWindow      _window;
    std::unique_ptr<internal::Clock>        _clock{std::make_unique<internal::Clock_Realtime>()};
    internal::RectRenderer                  _rect_renderer;
    internal::TriangleRenderer              _triangle_renderer;
    internal::TextRenderer                  _text_renderer;
    internal::TransformStack                _transform_stack{};
    ImageSize                               _framebuffer_size;
    ImageSize                               _window_size;
    glm::vec2                               _mouse_position{};
    glm::vec2                               _mouse_position_delta{0.f, 0.f};
    glm::vec2                               _drag_start_position{};
    bool                                    _is_dragging{false};
    std::optional<std::chrono::nanoseconds> _capped_delta_time{std::nullopt};
    std::chrono::steady_clock::time_point   _last_update{};
    Canvas                                  _main_canvas{{1, 1}};
    CanvasSizeMode                          _main_canvas_size_mode{CanvasSizeMode_SameAsWindow{}};
    std::reference_wrapper<Canvas>          _current_canvas{_main_canvas};
    bool                                    _window_is_fullscreen{false};
    int                                     _window_pos_x_before_fullscreen{};
    int                                     _window_pos_y_before_fullscreen{};
    int                                     _window_width_before_fullscreen;
    int                                     _window_height_before_fullscreen;
    Shader                                  _rect_shader{R"(
#version 410

in vec2 _raw_uv;
in vec2 _canvas_uv;
out vec4 _frag_color;

uniform bool _is_image;
uniform sampler2D _image;
uniform bool _is_ellipse;
uniform vec4 _fill_color;
uniform vec4 _stroke_color;
uniform float _stroke_weight;
uniform bool _use_stroke;
uniform vec2 _size;

// Thanks to https://iquilezles.org/www/articles/ellipsedist/ellipsedist.htm
float sdEllipse(  vec2 p,  vec2 ab ) {
    p = abs( p );
    bool s = dot(p/ab,p/ab)>1.0;
    float w = s ? atan(p.y*ab.x, p.x*ab.y) : 
                  ((ab.x*(p.x-ab.x)<ab.y*(p.y-ab.y))? 1.5707963 : 0.0);
    // find root with Newton solver
    for( int i=0; i<5; i++ ) {
        vec2 cs = vec2(cos(w),sin(w));
        vec2 u = ab*vec2( cs.x,cs.y);
        vec2 v = ab*vec2(-cs.y,cs.x);
        w = w + dot(p-u,v)/(dot(p-u,u)+dot(v,v));
    }
    return length(p-ab*vec2(cos(w),sin(w))) * (s?1.0:-1.0);
}

void main() {
    float dist;
    if (_is_ellipse) {
        dist = -sdEllipse(_canvas_uv, _size);
    }
    else /*is_rect*/ { 
        vec2 dd = _size - abs(_canvas_uv);
        dist = min(dd.x, dd.y);
    }

    const float m = 0.0005;
    if (_is_image) {
        _frag_color = texture(_image, _raw_uv);
    }
    else {
        // Fill vs Stroke
        if (_use_stroke) {
            float t = smoothstep(-m, m, _stroke_weight - dist);
            _frag_color = vec4(mix(_fill_color, _stroke_color, t));
        }
        else {
            _frag_color = _fill_color;
        }
    }
    
    // Shape
    float shape_factor = _is_ellipse ? smoothstep(-m, m, dist)
                        /*is_rect*/  : 1.;
    _frag_color *= shape_factor;
}
    )"};
    Shader                                  _line_shader{R"(
#version 410
out vec4 _frag_color;

in vec2 _uniform_uv;
in vec2 _raw_uv;
in vec2 _canvas_uv;

uniform float _aspect_ratio;
uniform vec4 _material;

void main()
{
    _frag_color = _material;

    vec2 uv = abs(_uniform_uv);
    float cap_center_x = _aspect_ratio - 1.;
    _frag_color *= uv.x > cap_center_x 
                    ? smoothstep(-0.0005, 0.0005,
                                 1. - length(uv - vec2(cap_center_x, 0.)))
                    : 1.;
}
    )"};
};

} // namespace p6
