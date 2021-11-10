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

UniqueGlfwWindow::UniqueGlfwWindow()
{
    WindowFactory::init();
    _window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!_window) {
        glfwTerminate();
        throw std::runtime_error("[p6::UniqueGlfwWindow] Failed to create a window");
    }
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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