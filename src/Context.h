#pragma once

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <functional>
#include <glm/glm.hpp>
#include <glpp/extended.hpp>
#include <memory>
#include <optional>
#include <stdexcept>
#include "Canvas.h"
#include "Color.h"
#include "Image.h"
#include "Key.h"
#include "MouseButton.h"
#include "MouseDrag.h"
#include "MouseMove.h"
#include "MouseScroll.h"
#include "Shader.h"
#include "Transform2D.h"
#include "details/RectRenderer.h"
#include "details/TextRenderer.h"
#include "details/Time/Clock.h"
#include "details/Time/Clock_FixedTimestep.h"
#include "details/Time/Clock_Realtime.h"
#include "details/UniqueGlfwWindow.h"

namespace p6 {

struct FullScreen {
};

struct FitX {
};

struct FitY {
};

class Context {
public:
    Context(WindowCreationParams window_creation_params = {});

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

    /// This function is called repeatedly. The framerate will be capped at your monitors refresh rate (60 frames per second on a typical monitor).
    std::function<void()> update = []() {};
    /// In this function you can render all the ImGui windows you want.
    std::function<void()> imgui = []() {};
    /// This function is called whenever the mouse is moved
    std::function<void(MouseMove)> mouse_moved = [](MouseMove) {};
    /// This function is called whenever the mouse is dragged
    std::function<void(MouseDrag)> mouse_dragged = [](MouseDrag) {};
    /// This function is called whenever a mouse button is pressed
    std::function<void(MouseButton)> mouse_pressed = [](MouseButton) {};
    /// This function is called whenever a mouse button is released
    std::function<void(MouseButton)> mouse_released = [](MouseButton) {};
    /// This function is called whenever the mouse wheel is scrolled
    std::function<void(MouseScroll)> mouse_scrolled = [](MouseScroll) {};
    /// This function is called whenever a keyboard key is pressed
    std::function<void(Key)> key_pressed = [](Key) {};
    /// This function is called whenever a keyboard key is released
    std::function<void(Key)> key_released = [](Key) {};
    /// This function is called repeatedly whenever a keyboard key is held. (NB: this only starts after holding the key for a little while. The axact behaviour is OS-specific)
    ///
    /// :warning: This is less than ideal to do things like handling the movement of a character. You should rather do, in your update function:
    ///
    /// ```
    /// if (p6.is_held(PhysicalKey::W)) { // TODO implement is_held and PhysicalKey and LogicalKey
    ///     character.move_forward(p6.delta_time());
    /// }
    /// ```
    std::function<void(Key)> key_repeated = [](Key) {};
    /// This function is called whenever an error occurs.
    std::function<void(std::string&&)> on_error = [](std::string&& error_message) {
        throw std::runtime_error{error_message};
    };
    /// This function is called whenever the framebuffer is resized.
    /// If you call framebuffer_size(), framebuffer_width(), framebuffer_height() or aspect_ratio() inside framebuffer_resized() they will already be referring to the new size.
    std::function<void()> framebuffer_resized = []() {};

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
    /// Draws an image as big as possible on the screen. This will respect the aspect ratio of the image.
    void image(const ImageOrCanvas&, FitX);
    void image(const ImageOrCanvas&, FitY = {});
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
    /** \defgroup canvas Canvas
     * You can either draw directly to the screen (the default) or onto a Canvas.
     * @{*/
    /* ------------------------------- */

    /// Sets the canvas where all the drawing commands will happen on
    void render_to_canvas(Canvas& canvas);
    /// Reset the Context to render to the screen
    void render_to_screen();

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

    /// Returns true iff the CTRL key is pressed (or CMD on Mac)
    bool ctrl() const;
    /// Returns true iff the SHIFT key is pressed
    bool shift() const;
    /// Returns true iff the ALT key is pressed
    bool alt() const;

    /**@}*/
    /* ------------------------------- */
    /** \defgroup window Window
     * Query the state of the window and control it.
     * @{*/
    /* ------------------------------- */

    /// Returns the aspect ratio of the window (a.k.a. width / height).
    float aspect_ratio() const;
    /// Returns the inverse aspect ratio of the window (a.k.a. height / width).
    float inverse_aspect_ratio() const;
    /// Returns the size of the framebuffer (width and height).
    ImageSize framebuffer_size() const;
    /// Returns the width of the framebuffer.
    int framebuffer_width() const;
    /// Returns the height of the framebuffer.
    int framebuffer_height() const;
    /// Returns the color of the pixel at the given position.
    /// The coordinates are expressed in the usual p6 coordinate system.
    /// The pixel is read from the current render target (which will be the screen in most cases, unless you used render_to_canvas())
    Color read_pixel(glm::vec2 position) const;
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

    /**@}*/
    /* ------------------------------- */
    /** \defgroup time Time
     * Query time information and control how the time evolves.
     * @{*/
    /* ------------------------------- */

    /// Returns the time in seconds since the creation of the Context.
    float time() const;

    /// Returns the time in seconds since the last update() call (or 0 if this is the first update).
    float delta_time() const;

    /// Sets the time mode as *realtime*.
    /// This means that what is returned by time() and delta_time() corresponds to the actual time that elapsed in the real world.
    /// This is ideal when you want to do realtime animation and interactive sketches.
    void set_time_mode_realtime();

    /// Sets the time mode as *fixedstep*.
    /// This means that what is returned by time() and delta_time() corresponds to an ideal world where there is exactly `1/framerate` seconds between each updates.
    /// This is ideal when you are exporting a video and don't want the long export time to influence your animation.
    /// `framerate` is expressed in frames per second
    void set_time_mode_fixedstep(float framerate);

    /// Makes sure that the framerate is adapted to your monitor: it will be 60 fps if you have a 60 Herz monitor (which is the most common), or 120 fps if you have a 120 Hertz monitor, etc.
    /// This is the default framerate mode.
    void framerate_synced_with_monitor();

    /// Makes sure that the framerate will never go above the specified value.
    void framerate_capped_at(float framerate);

    /// Removes any limit on the framerate. update() will be called as fast as possible.
    void framerate_as_fast_as_possible();

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
    ImageSize window_size() const { return _window_size; }
    glm::vec2 window_to_relative_coords(glm::vec2 pos) const;

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

    void set_vertex_shader_uniforms(const Shader& shader, Transform2D transform) const;
    void render_with_rect_shader(Transform2D transform, bool is_ellipse, bool is_image) const;

    Transform2D make_transform_2D_impl(glm::vec2 offset_to_center, glm::vec2 corner_position, Radii radii, Rotation rotation) const;
    Transform2D make_transform_2D(FullScreen) const;

private:
    mutable details::UniqueGlfwWindow       _window;
    std::unique_ptr<details::Clock>         _clock{std::make_unique<details::Clock_Realtime>()};
    details::RectRenderer                   _rect_renderer;
    details::TextRenderer                   _text_renderer;
    ImageSize                               _framebuffer_size;
    ImageSize                               _window_size;
    glm::vec2                               _mouse_position;
    glm::vec2                               _mouse_position_delta{0.f, 0.f};
    glm::vec2                               _drag_start_position{};
    bool                                    _is_dragging{false};
    std::optional<std::chrono::nanoseconds> _capped_delta_time{std::nullopt};
    std::chrono::steady_clock::time_point   _last_update{};
    Canvas                                  _default_canvas{{1, 1}};
    Shader                                  _rect_shader{R"(
#version 330

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
#version 330
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
