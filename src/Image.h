#pragma once

#include <glpp/extended.hpp>

namespace p6 {

using ImageSize = glpp::ImageSize;
using Texture   = glpp::Texture;

class Image {
public:
    explicit Image(ImageSize size, const void* data = nullptr);

    /// Returns the size in pixels of the image
    ImageSize size() const { return _render_target.size(); }

    /// Returns the internal texture, that you can use to do advanced stuff with custom shaders
    const Texture& texture() const { return _render_target.texture(); }

    /// Resizes the image
    /// /!\ All of its content is lost. If you don't want that you can use resize() instead
    void destructive_resize(ImageSize size) { _render_target.resize(size); }
    /// Resizes the image
    void resize(ImageSize size) { _render_target.conservative_resize(size); }

private:
    friend class Context;
    glpp::RenderTarget _render_target;
};

Image load_image(const char* file_path);

} // namespace p6