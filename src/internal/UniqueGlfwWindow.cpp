#include "UniqueGlfwWindow.h"
#include <glpp/glpp.hpp>
#include <stdexcept>

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Required on MacOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on MacOS
    _window = glfwCreateWindow(window_creation_params.width,
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
    }
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