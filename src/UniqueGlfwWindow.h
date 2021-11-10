#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace p6 {
namespace details {

class UniqueGlfwWindow {
public:
    UniqueGlfwWindow();
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
