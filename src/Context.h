#pragma once

#include "UniqueGlfwWindow.h"

namespace p6 {

class Context {
public:
    Context();
    void run();

private:
    details::UniqueGlfwWindow _window;
};

} // namespace p6
