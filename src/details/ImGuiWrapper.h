#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace p6::internal::ImGuiWrapper {

void create_context();
void setup_for_glfw(GLFWwindow* window);
void begin_frame();
void end_frame(GLFWwindow* window);

} // namespace p6::internal::ImGuiWrapper
