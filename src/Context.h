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

    std::function<void()> update = []() {};

    void background(Color color) const;
    /// Returns the time in seconds since the creation of the Context
    float time() const;
    /// Returns the time in seconds since the last update() call (or 0 if this is the first update)
    float delta_time() const;

private:
    details::UniqueGlfwWindow       _window;
    std::unique_ptr<details::Clock> _clock = std::make_unique<details::Clock_Realtime>();
};

} // namespace p6
