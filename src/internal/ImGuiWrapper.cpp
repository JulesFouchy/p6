#include "ImGuiWrapper.h"
#include <glad/gl.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

namespace p6::internal {

static void create_context(ImGuiConfigFlags config_flags)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= config_flags;
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplOpenGL3_Init("#version 410");
}

static void setup_for_glfw(GLFWwindow* window)
{
    ImGui_ImplGlfw_InitForOpenGL(window, true);
}

ImGuiWrapper::ImGuiWrapper(GLFWwindow* window, ImGuiConfigFlags config_flags)
{
    static bool is_first = true;
    _owns_imgui_context  = is_first;
    is_first             = false;
    if (_owns_imgui_context)
    {
        create_context(config_flags);
        setup_for_glfw(window);
    }
}

ImGuiWrapper::~ImGuiWrapper()
{
    if (!_owns_imgui_context || _moved_from)
        return;

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
}

ImGuiWrapper::ImGuiWrapper(ImGuiWrapper&& other) noexcept
    : _owns_imgui_context{other._owns_imgui_context}
{
    other._moved_from = true;
}
ImGuiWrapper& ImGuiWrapper::operator=(ImGuiWrapper&& other) noexcept
{
    _moved_from         = false;
    other._moved_from   = true;
    _owns_imgui_context = other._owns_imgui_context;
    return *this;
}

void ImGuiWrapper::begin_frame()
{
    if (!_owns_imgui_context || _moved_from)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiWrapper::end_frame(GLFWwindow* window)
{
    if (!_owns_imgui_context || _moved_from)
        return;

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)*
    const auto& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

} // namespace p6::internal
