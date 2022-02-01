#pragma once

#include <glpp/extended.hpp>

namespace p6 {

using ImageSize = glpp::ImageSize;
using Texture   = glpp::Texture;

/* ------------------------------- */
/** \defgroup image Image
 * Create, modify and query information about images.
 * @{*/
/* ------------------------------- */

/// An image can be drawn onto, and can be displayed.
/// If you plan on drawing on the image, you can create it without data (it will create a black and transparent image).
/// If you want to load an image, you can use load_image() instead of the constructor.
class Image {
public:
    /// data must be nullptr, or an array of size `size.width() * size.height() * 4`, with R, G, B and A channels, starting with the bottom left pixel, and going row by row.
    explicit Image(ImageSize size, const uint8_t* data = nullptr);

    /// Returns the size in pixels of the image.
    ImageSize size() const { return _render_target.size(); }

    /// Returns the internal texture, that you can use to do advanced stuff with custom shaders.
    const Texture& texture() const { return _render_target.texture(); }

    /// Resizes the image.
    /// :warning: All of its content is lost. If you don't want that you can use resize() instead.
    void destructive_resize(ImageSize size) { _render_target.resize(size); }
    /// Resizes the image.
    void resize(ImageSize size) { _render_target.conservative_resize(size); }

private:
    friend class Context;
    glpp::RenderTarget _render_target;
};

/// Loads an image from a file.
/// Throws a `std::runtime_error` if the file doesn't exist or isn't a valid image file.
Image load_image(const char* file_path);

/**@}*/

} // namespace p6