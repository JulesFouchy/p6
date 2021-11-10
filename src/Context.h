#pragma once

#include "Color.h"
#include "UniqueGlfwWindow.h"

namespace p6 {

class Context {
public:
    Context(WindowCreationParams window_creation_params = {});
    void run();

    void background(Color color) const;

private:
    details::UniqueGlfwWindow _window;
};

} // namespace p6
