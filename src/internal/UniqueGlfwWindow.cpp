#include "UniqueGlfwWindow.h"
#include <glpp/glpp.hpp>
#include <iostream>
#include <stdexcept>
#include "OpenGL_Debug.h"

namespace p6::internal {

class WindowFactory {
public:
    static void init()
    {
        static WindowFactory instance{};
    }
    WindowFactory(const WindowFactory&)            = delete;
    WindowFactory& operator=(const WindowFactory&) = delete;
    WindowFactory(WindowFactory&&)                 = delete;
    WindowFactory& operator=(WindowFactory&&)      = delete;

private:
    WindowFactory()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("[p6::WindowFactory] Failed to intialize glfw");
        }
    }
    ~WindowFactory()
    {
        glpp::shut_down();
        glfwTerminate();
    }
};

UniqueGlfwWindow::UniqueGlfwWindow(WindowCreationParams window_creation_params)
{
    WindowFactory::init();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#if !defined(__APPLE__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 4.3 allows us to use improved debugging. But it is not available on MacOS.
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
#if OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Required on MacOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on MacOS
    _window = glfwCreateWindow(window_creation_params.width,       // NOLINT(cppcoreguidelines-prefer-member-initializer)
                               window_creation_params.height,
                               window_creation_params.title,
                               nullptr, nullptr);
    if (!_window)
    {
        throw std::runtime_error("[p6::UniqueGlfwWindow] Failed to create a window");
    }
    glfwMakeContextCurrent(_window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        throw std::runtime_error("[p6::UniqueGlfwWindow] Failed to initialize glad");
    }

#if OPENGL_DEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&p6::GLDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else
    {
        std::cout << "Unable to create an OpenGL debug context.\n";
    }
#endif
}

UniqueGlfwWindow::~UniqueGlfwWindow()
{
    glfwDestroyWindow(_window);
}

UniqueGlfwWindow::UniqueGlfwWindow(UniqueGlfwWindow&& rhs) noexcept
    : _window{rhs._window}
{
    rhs._window = nullptr;
}

UniqueGlfwWindow& UniqueGlfwWindow::operator=(UniqueGlfwWindow&& rhs) noexcept
{
    if (this != &rhs)
    {
        _window     = rhs._window;
        rhs._window = nullptr;
    }
    return *this;
}

} // namespace p6::internal