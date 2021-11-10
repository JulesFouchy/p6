#pragma once

#include <functional>
#include <memory>
#include "Color.h"
#include "Time/Clock.h"
#include "Time/Clock_FixedTimestep.h"
#include "Time/Clock_Realtime.h"
#include "UniqueGlfwWindow.h"

namespace p6 {

class Context {
public:
    Context(WindowCreationParams window_creation_params = {});
    void run();

    /* ---------------------------------------- *
     * ---------CUSTOMIZABLE FUNCTIONS--------- *
     * ---------------------------------------- */

    /// This function is called repeatedly, once every 1/framerate() seconds (or at least it will try, if your update() code is too slow then the next updates will necessarily be delayed).
    std::function<void()> update = []() {};

    /* ------------------------- *
     * ---------DRAWING--------- *
     * ------------------------- */

    /// Sets the color of each pixel of the canvas.
    /// NB: No blending is applied ; even if you specify an alpha of 0.5 the old canvas is completely erased. This means that setting an alpha here doesn't matter much. It is only meaningful if you export the canvas as a png, or if you try to blend the canvas on top of another image.
    void background(Color color) const;

    /* ---------------------- *
     * ---------TIME--------- *
     * ---------------------- */

    /// Returns the time in seconds since the creation of the Context
    float time() const;

    /// Returns the time in seconds since the last update() call (or 0 if this is the first update)
    float delta_time() const;

    /// Sets the time_mode as realtime.
    /// This means that what is returned by time() and delta_time() corresponds to the actual time that elapsed in the real world.
    /// This is ideal when you want to do realtime animation and interactive sketches.
    void set_time_mode_realtime();

    /// Sets the time_mode as fixedstep.
    /// This means that what is returned by time() and delta_time() corresponds to an ideal world where there is exactly 1/framerate seconds between each updates.
    /// This is ideal when you are exporting a video and don't want the long export time to influence your animation.
    void set_time_mode_fixedstep();

    /* ------------------------------- *
     * ---------MISCELLANEOUS--------- *
     * ------------------------------- */

    /// Starts the loop again if it was paused with no_loop()
    void loop();
    /// Pauses the loop. No update() will be called, until you call loop(). User inputs are still processed.
    void no_loop();
    /// Returns true iff we are currently looping. See loop() and no_loop()
    bool is_looping() const;

private:
    details::UniqueGlfwWindow       _window;
    std::unique_ptr<details::Clock> _clock = std::make_unique<details::Clock_Realtime>();
};

} // namespace p6
