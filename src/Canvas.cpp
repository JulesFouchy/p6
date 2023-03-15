#include "Canvas.h"
#include <img/img.hpp>
#include "internal/append_number_if_file_already_exists.h"
#include "internal/make_directories_if_necessary.h"
#include "make_absolute_path.h"

namespace p6 {

Canvas::Canvas(ImageSize size, glpp::TextureLayout texture_layout)
    : _render_target{size, nullptr, texture_layout, true}
{}

void save_image(const Canvas& canvas, std::filesystem::path path)
{
    const auto automatically_restore_previous_bindings_at_end_of_scope = glpp::RenderTargetBindState_RAII{};
    canvas.render_target().bind();
    const auto                 width  = static_cast<img::Size::DataType>(canvas.size().width());
    const auto                 height = static_cast<img::Size::DataType>(canvas.size().height());
    std::unique_ptr<uint8_t[]> data{new uint8_t[4 * width * height]};
    glReadPixels(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGBA, GL_UNSIGNED_BYTE, data.get());
    const auto absolute_path = internal::append_number_if_file_already_exists(make_absolute_path(path));
    internal::make_directories_if_necessary(absolute_path);
    if (path.extension() == ".png")
    {
        img::save_png(absolute_path.string().c_str(), width, height, data.get(), 4);
    }
    else if (path.extension() == ".jpg"
             || path.extension() == ".jpeg")
    {
        img::save_jpeg(absolute_path.string().c_str(), width, height, data.get(), 4);
    }
    else
    {
        throw std::runtime_error{"[p6::save_image] Only supports .png, .jpeg and .jpg extensions"};
    }
}

} // namespace p6