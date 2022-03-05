#pragma once

#include <glpp/extended.hpp>
#include "ImageCommon.h"

namespace p6 {

/* ------------------------------- */
/** \defgroup image Image
 * Create, modify and query information about images.
 * @{*/
/* ------------------------------- */

class Image {
public:
    /// Creates an Canvas filled with data.
    /// data must be an array of size `size.width() * size.height() * 4`, with R, G, B and A channels, starting with the bottom left pixel, and going row by row.
    /// texture_layout is an advanced setting; it controls how the pixels are gonna be stored on the GPU.
    explicit Image(ImageSize size, const uint8_t* data,
                   glpp::TextureLayout texture_layout = {glpp::InternalFormat::RGBA8, glpp::Channels::RGBA, glpp::TexelDataType::UnsignedByte});

    /// Returns the aspect ratio of the canvas (`width / height`)
    float aspect_ratio() const { return _texture.size().aspect_ratio(); }

private:
    glpp::Texture2D _texture;
};

/// Loads an image from a file.
/// Throws a `std::runtime_error` if the file doesn't exist or isn't a valid image file.
Image load_image(const char* file_path);

/**@}*/

} // namespace p6