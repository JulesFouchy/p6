#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace p6 {

struct WindowCreationParams {
    int         width  = 1280;
    int         height = 720;
    const char* title  = "p6";
};

namespace details {

class UniqueGlfwWindow {
public:
    UniqueGlfwWindow(WindowCreationParams window_creation_params);
    ~UniqueGlfwWindow();
    UniqueGlfwWindow(const UniqueGlfwWindow&) = delete;
    UniqueGlfwWindow& operator=(const UniqueGlfwWindow&) = delete;
    UniqueGlfwWindow(UniqueGlfwWindow&& rhs) noexcept;
    UniqueGlfwWindow& operator=(UniqueGlfwWindow&& rhs) noexcept;

    GLFWwindow*       operator*() { return _window; }
    const GLFWwindow* operator*() const { return _window; }

private:
    GLFWwindow* _window;
};

} // namespace details
} // namespace p6
