#pragma once
#include <imgui/imgui.h>
#include "glfw.h"

namespace p6 {

struct WindowCreationParams {
    int         width  = 1280;
    int         height = 720;
    const char* title  = "p6";
    ImGuiConfigFlags imgui_config_flags = ImGuiConfigFlags_NavEnableKeyboard  // Enable Keyboard Controls
                                          | ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
};

namespace internal {
class UniqueGlfwWindow {
public:
    explicit UniqueGlfwWindow(WindowCreationParams window_creation_params);
    ~UniqueGlfwWindow();
    UniqueGlfwWindow(const UniqueGlfwWindow&) = delete;
    UniqueGlfwWindow& operator=(const UniqueGlfwWindow&) = delete;
    UniqueGlfwWindow(UniqueGlfwWindow&& rhs) noexcept;
    UniqueGlfwWindow& operator=(UniqueGlfwWindow&& rhs) noexcept;

    GLFWwindow*       operator*() { return _window; }
    const GLFWwindow* operator*() const { return _window; }

private:
    GLFWwindow* _window{};
};

} // namespace internal
} // namespace p6
