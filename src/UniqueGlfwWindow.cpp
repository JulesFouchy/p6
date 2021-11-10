#include "UniqueGlfwWindow.h"
#include <glad/glad.h>
#include <stdexcept>

namespace p6 {
namespace details {

class WindowFactory {
public:
    static void init()
    {
        static WindowFactory instance{};
    }
    WindowFactory(const WindowFactory&) = delete;
    WindowFactory& operator=(const WindowFactory&) = delete;
    WindowFactory(WindowFactory&&)                 = delete;
    WindowFactory& operator=(WindowFactory&&) = delete;

private:
    WindowFactory()
    {
        if (!glfwInit()) {
            throw std::runtime_error("[p6::WindowFactory] Failed to intialize glfw");
        }
    }
    ~WindowFactory()
    {
        glfwTerminate();
    }
};

UniqueGlfwWindow::UniqueGlfwWindow(WindowCreationParams window_creation_params)
{
    WindowFactory::init();
    _window = glfwCreateWindow(window_creation_params.width,
                               window_creation_params.height,
                               window_creation_params.title,
                               nullptr, nullptr);
    if (!_window) {
        glfwTerminate();
        throw std::runtime_error("[p6::UniqueGlfwWindow] Failed to create a window");
    }
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) { // NOLINT
        throw std::runtime_error("[p6::UniqueGlfwWindow] Failed to intialize glad");
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
    if (this != &rhs) {
        _window     = rhs._window;
        rhs._window = nullptr;
    }
    return *this;
}

} // namespace details
} // namespace p6