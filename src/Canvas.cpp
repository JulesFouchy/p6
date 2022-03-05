#include "Canvas.h"

namespace p6 {

Canvas::Canvas(ImageSize size, glpp::TextureLayout texture_layout)
    : _render_target{size, nullptr, texture_layout}
{
}

} // namespace p6