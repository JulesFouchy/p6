#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace p6::internal::ImGuiWrapper
{
void initialize(GLFWwindow* window);
void shut_down();

void begin_frame();
void end_frame(GLFWwindow* window);

} // namespace p6::internal::ImGuiWrapper
