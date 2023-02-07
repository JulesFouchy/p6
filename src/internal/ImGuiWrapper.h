#pragma once

#include "glfw.h"

namespace p6::internal::ImGuiWrapper {

void initialize(GLFWwindow* window);

class Raii {
public:
    Raii() = default;
    ~Raii();

    Raii(Raii&&) noexcept;
    Raii& operator=(Raii&&) noexcept;

    Raii(const Raii&) = delete;
    Raii& operator=(const Raii&) = delete;

private:
    bool _moved_from{false};
};

void begin_frame();
void end_frame(GLFWwindow* window);

} // namespace p6::internal::ImGuiWrapper
