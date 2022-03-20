#pragma once

#include <glpp/extended.hpp>

namespace p6 {

using ImageSize = glpp::ImageSize;
using Texture   = glpp::Texture2D;

class ImageOrCanvas {
public:
    virtual const glpp::Texture2D& texture() const      = 0;
    virtual float                  aspect_ratio() const = 0;
};

} // namespace p6