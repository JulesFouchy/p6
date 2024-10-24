#include "TextRenderer.h"
#include <exe_path/exe_path.h>
#include <stb_image/stb_image.h>
#include <algorithm>
#include <glpp/glpp.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../Shader.h"
#include "CharCorrespondanceTable.h"
#include "font_atlas"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(*(A)))

namespace p6::internal {

static Image load_font_atlas()
{
    stbi_set_flip_vertically_on_load(1);
    int         w, h;
    const auto* data = stbi_load_from_memory(font_atlas, ARRAY_SIZE(font_atlas), &w, &h, nullptr, 4);
    return Image({static_cast<GLsizei>(w),
                  static_cast<GLsizei>(h)},
                 data);
}

TextRenderer::TextRenderer()
    : _font_atlas{load_font_atlas()}
{
}

static void convert_and_copy_text_to_buffer(const std::u16string& text, TextRenderer::ArrayOfUint8& cpu_buffer)
{
    std::transform(text.begin(), text.end(), cpu_buffer.begin(),
                   [](char16_t c) {
                       const auto search = char_correspondance.find(c);
                       return search != char_correspondance.end()
                                  ? search->second
                                  : static_cast<uint8_t>(63); // Render unknown characters as '?';
                   });
}

static void send_text_buffer_to_gpu(glpp::Texture1D& gpu_buffer, const TextRenderer::ArrayOfUint8& cpu_buffer, GLsizei actual_buffer_size)
{
    gpu_buffer.upload_data(
        actual_buffer_size,
        cpu_buffer.data(),
        {glpp::InternalFormat::R8UI,
         glpp::Channels::R_Integer,
         glpp::TexelDataType::UnsignedByte});
}

void TextRenderer::setup_rendering_for(const std::u16string& text, Color color, float inflating)
{
    if (text.length() > 1024)
    { // TODO do we need to handle arbitrarily long text?
        throw std::runtime_error("[p6::TextRenderer] This text is too long to be rendered");
    }

    convert_and_copy_text_to_buffer(text, _cpu_text_buffer);
    send_text_buffer_to_gpu(_gpu_text_buffer, _cpu_text_buffer, static_cast<GLsizei>(text.length()));

    _font_atlas.texture().bind_to_texture_unit(0);
    _gpu_text_buffer.bind_to_texture_unit(1);

    _shader.set("_font_atlas", 0);
    _shader.set("_text_buffer", 1);
    _shader.set("_sentence_size", static_cast<float>(text.length()));
    _shader.set("_inflating", inflating);
    _shader.set("_color", color.as_premultiplied_vec4());
}

namespace TextRendererU {
Radii compute_text_radii(const std::u16string& text, float font_size)
{
    return font_size * glm::vec2{static_cast<float>(text.length()), 1.f};
}
} // namespace TextRendererU

} // namespace p6::internal