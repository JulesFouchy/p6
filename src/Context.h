#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glpp/extended.hpp>
#include <memory>
#include <stdexcept>
#include "Color.h"
#include "Image.h"
#include "KeyEvent.h"
#include "MouseButton.h"
#include "MouseDrag.h"
#include "MouseMove.h"
#include "MouseScroll.h"
#include "Shader.h"
#include "Transform2D.h"
#include "details/RectRenderer.h"
#include "details/Time/Clock.h"
#include "details/Time/Clock_FixedTimestep.h"
#include "details/Time/Clock_Realtime.h"
#include "details/UniqueGlfwWindow.h"

namespace p6 {

struct Center {
    glm::vec2 value;

    Center(glm::vec2 value)
        : value{value} {}
};

struct Radii {
    glm::vec2 value;

    Radii(float x, float y)
        : value{x, y} {}

    Radii(glm::vec2 v)
        : value{v} {}
};

struct Radius {
    float value;
};

struct Rotation {
    Angle value;
};

class Context {
public:
    Context(WindowCreationParams window_creation_params = {});

    /* ------------------------------- */
    /** \defgroup events Events
     * You can set those functions as you wish to react to various events.
     * The simplest way is to use a lambda:
     * 
     * ```cpp
     * auto ctx = p6::Context{};
     * ctx.mouse_pressed = [](p6::MouseButton) {
     *     std::cout << "Hello World\n";
     * };
     * ```
     * @{*/
    /* ------------------------------- */

    /// This function is called repeatedly. The framerate will be capped at your monitors refresh rate (60 frames per second on a typical monitor).
    std::function<void()> update = []() {};
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
    std::function<void(KeyEvent)> key_pressed = [](KeyEvent) {};
    /// This function is called whenever a keyboard key is released
    std::function<void(KeyEvent)> key_released = [](KeyEvent) {};
    /// This function is called repeatedly whenever a keyboard key is held. (NB: this only starts after holding the key for a little while. The axact behaviour is OS-specific)
    /// /!\ This is less than ideal to do things like handling the movement of a character. You should rather do, in your update function:
    ///
    /// ```cpp
    /// if (p6.is_held(PhysicalKey::W)) { // TODO implement is_held and PhysicalKey and LogicalKey
    ///     character.move_forward(p6.delta_time());
    /// }
    /// ```
    std::function<void(KeyEvent)> key_repeated = [](KeyEvent) {};
    /// This function is called whenever an error occurs.
    std::function<void(std::string&&)> on_error = [](std::string&& error_message) {
        throw std::runtime_error{error_message};
    };

    /**@}*/
    /* ------------------------------- */
    /** \defgroup drawing Drawing
     * All the functions to draw shapes and control the look of them.
     * @{*/
    /* ------------------------------- */

    Color fill{1.f, 1.f, 1.f, 0.5f};
    Color stroke{0.f, 0.f, 0.f};
    float stroke_weight = 0.01f;

    /// Sets the color and alpha of each pixel of the canvas.
    /// NB: No blending is applied; even if you specify an alpha of 0.5 the old canvas is completely erased. This means that setting an alpha here doesn't matter much. It is only meaningful if you export the canvas as a png, or if you later try to blend the canvas on top of another image.
    void background(Color color) const;

    /**
     * Draws a rectangle.
     * \param params Shape of the rect
     * 
     * ```
     * p6.fill = {0.f, 0.8f, 0.3f};
     * p6.rectangle({});
     * ```
     * 
     */
    void rectangle(Transform2D transform) const;
    /// Draws a circle
    void circle(Center center, Radius radius) const;
    /// Draws an ellipse
    void ellipse(Center center, Radii radii, Rotation rotation) const;
    void ellipse(Center center, Radius radius) const;
    void ellipse(Transform2D transform) const;
    /// Draws an image
    void image(const Image& img, Transform2D transform) const;

    /**@}*/
    /* ------------------------------- */
    /** \defgroup rendering_destination Rendering Destination
     * Controls where the rendering happens. You can either draw directly to the screen (the default) or onto an image.
     * 
     * ```cpp
     * auto ctx = p6::Context{};
     * auto my_image = p6::Image{{1000, 1000}}; // Creates an empty image of size 1000x1000
     * ctx.render_to_image(my_image);
     * ctx.rectangle({}); // Draws on my_image
     * ctx.ellipse({});   // Draws on my_image again
     * ctx.render_to_screen();
     * ctx.rectangle({});       // Draws on the screen
     * ctx.image(my_image, {}); // Draws my_image onto the screen
     * ```
     * @{*/
    /* ------------------------------- */

    /// Sets the image where all the drawing commands will happen on
    void render_to_image(Image& image);
    /// Reset the Context to render to the screen
    void render_to_screen();

    /**@}*/
    /* ------------------------------- */
    /** \defgroup input Input
     * Allows you to query the state of the mouse and the keyboard.
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
     * Allows you to query the state of the window.
     * @{*/
    /* ------------------------------- */

    /// Returns the aspect ratio of the window (a.k.a. width / height).
    float aspect_ratio() const;
    /// Returns the size of the window (width and height).
    ImageSize window_size() const;
    /// Returns the width of the window.
    int window_width() const;
    /// Returns the height of the window.
    int window_height() const;
    /// Returns true iff the window is currently focused.
    bool window_is_focused() const;
    /// Maximizes the window.
    void maximize_window();

    /**@}*/
    /* ------------------------------- */
    /** \defgroup time Time
     * Allows you to query time information and control how the time evolves.
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

    /**@}*/
    /* ------------------------------- */
    /** \defgroup update-flow Update Flow
     * Allows you to control the update() loop and query information about it.
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

    void render_with_rect_shader(Transform2D transform, bool is_ellipse, bool is_image) const;

private:
    mutable details::UniqueGlfwWindow _window;
    std::unique_ptr<details::Clock>   _clock = std::make_unique<details::Clock_Realtime>();
    details::RectRenderer             _rect_renderer;
    ImageSize                         _window_size;
    glm::vec2                         _mouse_position;
    glm::vec2                         _mouse_position_delta{0.f, 0.f};
    glm::vec2                         _drag_start_position{};
    bool                              _is_dragging = false;
    Image                             _default_render_target;
    Shader                            _rect_shader{R"(
#version 330

in vec2 _uv;
in vec2 _uv_canvas_scale;
out vec4 _frag_color;

uniform bool _is_image;
uniform sampler2D _image;
uniform bool _is_ellipse;
uniform vec4 _fill_color;
uniform vec4 _stroke_color;
uniform float _stroke_weight;
uniform vec2 _rect_size;

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
        dist = -sdEllipse(_uv_canvas_scale, _rect_size);
    }
    else /*is_rect*/ { 
        vec2 dd = _rect_size - abs(_uv_canvas_scale);
        dist = min(dd.x, dd.y);
    }

    const float m = 0.0005;
    if (_is_image) {
        _frag_color = texture(_image, _uv);
    }
    else {
        // Fill vs Stroke
        float t = smoothstep(-m, m, _stroke_weight - dist);
        _frag_color = vec4(mix(_fill_color, _stroke_color, t));
    }
    
    // Shape
    float shape_factor = _is_ellipse ? smoothstep(-m, m, dist)
                        /*is_rect*/  : 1.;
    _frag_color.a *= shape_factor;
}
    )"};
};

} // namespace p6
