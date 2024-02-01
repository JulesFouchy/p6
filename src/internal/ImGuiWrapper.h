#pragma once
#include <imgui/imgui.h>
#include "glfw.h"

namespace p6::internal {

class ImGuiWrapper {
public:
    ImGuiWrapper(GLFWwindow* window, ImGuiConfigFlags config_flags);
    ~ImGuiWrapper();

    ImGuiWrapper(ImGuiWrapper&&) noexcept;
    ImGuiWrapper& operator=(ImGuiWrapper&&) noexcept;

    ImGuiWrapper(const ImGuiWrapper&)            = delete;
    ImGuiWrapper& operator=(const ImGuiWrapper&) = delete;

    void begin_frame();
    void end_frame(GLFWwindow* window);

private:
    bool _moved_from{false};
    bool _owns_imgui_context{};
};

} // namespace p6::internal
