#pragma once

#include <filesystem>
#include <glpp/extended.hpp>
#include "ImageCommon.h"

namespace p6 {

/* ------------------------------- */
/** \defgroup image Image
 * Load and query information about images.
 * @{*/
/* ------------------------------- */

class Image : public ImageOrCanvas {
public:
    /// Creates an Image filled with data.
    /// This is for advanded uses; prefer using p6::load_image().
    /// data must be an array of size `size.width() * size.height() * 4`, with R, G, B and A channels, starting with the bottom left pixel, and going row by row.
    /// texture_layout is an advanced setting; it controls how the pixels are gonna be stored on the GPU.
    explicit Image(ImageSize size, const uint8_t* data,
                   glpp::TextureLayout texture_layout = {glpp::InternalFormat::RGBA8, glpp::Channels::RGBA, glpp::TexelDataType::UnsignedByte});

    /// Returns the size in pixels.
    ImageSize size() const { return _texture.size(); }
    /// Returns the aspect ratio (`width / height`)
    float aspect_ratio() const override { return _texture.size().aspect_ratio(); }
    /// Returns the inverse aspect ratio (`height / width`)
    float inverse_aspect_ratio() const { return _texture.size().inverse_aspect_ratio(); }

    const glpp::Texture2D& texture() const override { return _texture; }

private:
    glpp::Texture2D _texture;
};

/// Loads an image from a file.
/// If the path is relative, it will be relative to the directory containing your executable.
/// Throws a `std::runtime_error` if the file doesn't exist or isn't a valid image file.
[[nodiscard]] Image load_image(std::filesystem::path file_path);

/**@}*/

} // namespace p6