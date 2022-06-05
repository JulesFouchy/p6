#include "Canvas.h"
#include <img/img.hpp>
#include "internal/make_absolute_path.h"
#include "internal/make_directories_if_necessary.h"

namespace p6 {

Canvas::Canvas(ImageSize size, glpp::TextureLayout texture_layout)
    : _render_target{size, nullptr, texture_layout}
{}

void save(const Canvas& canvas, std::filesystem::path path)
{
    canvas.texture().bind();
    const auto                 width  = canvas.size().width();
    const auto                 height = canvas.size().height();
    std::unique_ptr<uint8_t[]> data{new uint8_t[4 * width * height]};
    glReadPixels(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGBA, GL_UNSIGNED_BYTE, data.get());
    const auto absolute_path = internal::make_absolute_path(path);
    internal::make_directories_if_necessary(absolute_path);
    img::save_png(absolute_path.string().c_str(), width, height, data.get(), 4);
}

} // namespace p6