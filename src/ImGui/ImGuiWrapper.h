#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace p6::internal {

void imgui_create_context();
void imgui_setup_for_glfw(GLFWwindow* window);
void imgui_new_frame();
void end_frame(GLFWwindow* window);

} // namespace p6::internal
