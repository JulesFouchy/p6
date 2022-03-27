#include "Canvas.h"
#include <img/img.hpp>

namespace p6 {

Canvas::Canvas(ImageSize size, glpp::TextureLayout texture_layout)
    : _render_target{size, nullptr, texture_layout}
{
}

void save_canvas(const Canvas& canvas, std::filesystem::path path)
{
    canvas.texture().bind();
    const auto                 width  = canvas.size().width();
    const auto                 height = canvas.size().height();
    std::unique_ptr<uint8_t[]> data{new uint8_t[4 * width * height]};
    glReadPixels(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGBA, GL_UNSIGNED_BYTE, data.get());
    img::save_png(path.string().c_str(), width, height, data.get(), 4);
}

} // namespace p6