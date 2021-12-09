#pragma once

#include <glpp/extended.hpp>

namespace p6 {

using Image     = glpp::RenderTarget;
using ImageSize = glpp::ImageSize;

Image load_image(const char* file_path);

} // namespace p6