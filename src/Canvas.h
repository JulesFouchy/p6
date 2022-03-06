#pragma once

#include "ImageCommon.h"

namespace p6 {

/// \ingroup canvas
/// A canvas is an image that can be drawn onto.
class Canvas : public ImageOrCanvas {
public:
    /// Creates an empty canvas
    /// texture_layout is an advanced setting; it controls how the pixels are gonna be stored on the GPU.
    explicit Canvas(ImageSize           size,
                    glpp::TextureLayout texture_layout = {glpp::InternalFormat::RGBA16, glpp::Channels::RGBA, glpp::TexelDataType::UnsignedByte});

    /// Returns the size in pixels.
    ImageSize size() const { return _render_target.size(); }
    /// Returns the aspect ratio (`width / height`)
    float aspect_ratio() const override { return size().aspect_ratio(); }
    /// Returns the inverse aspect ratio (`height / width`)
    float inverse_aspect_ratio() const { return size().inverse_aspect_ratio(); }

    /// Returns the internal texture, that you can use to do advanced stuff with custom shaders.
    const Texture& texture() const override { return _render_target.texture(); }
    /// Returns the internal render target, that you can use to do advanced stuff.
    const glpp::RenderTarget& render_target() const { return _render_target; }
    /// Returns the internal render target, that you can use to do advanced stuff.
    glpp::RenderTarget& render_target() { return _render_target; }

    /// Resizes the canvas.
    void resize(ImageSize size) { _render_target.conservative_resize(size); }
    /// Resizes the canvas.
    /// :warning: All of its pixel content is lost. If you don't want that you can use resize() instead.
    void destructive_resize(ImageSize size) { _render_target.resize(size); }

private:
    glpp::RenderTarget _render_target;
};

} // namespace p6