#include "Image.h"
#include <img/img.hpp>

namespace p6 {

Image::Image(ImageSize /*size*/, const uint8_t* /*data*/, glpp::TextureLayout /*texture_layout*/)
// : _render_target{size, data, texture_layout}
{
}

Image load_image(const char* file_path)
{
    const auto image_data = img::load(file_path, 4);
    return Image{{static_cast<GLsizei>(image_data.size().width()),
                  static_cast<GLsizei>(image_data.size().height())},
                 image_data.data()};
}

} // namespace p6