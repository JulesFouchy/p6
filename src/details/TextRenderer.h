#pragma once

#include <array>
#include <glpp/glpp.hpp>
#include <map>
#include <string>
#include "../Color.h"
#include "../Image.h"
#include "../NamedColor.h"
#include "../Shader.h"
#include "RectRenderer.h"

namespace p6 {
namespace details {

struct TextParams {
    float font_size = 0.1f;
    float inflating = 0.01f;
    Color color     = NamedColor::Black;
};

class TextRenderer {
public:
    TextRenderer();

    /// Prepares the shader to render the given text
    void          setup_rendering_for(const std::u16string& text, TextParams);
    Radii         compute_text_radii(const std::u16string& text, float font_size) const;
    const Shader& shader() const { return _shader; }

private:
    void          update_buffer_from_str(const std::u16string& text);
    void          update_data(const std::u16string& text);
    static size_t compute_sentence_size(const std::u16string& text);

    std::array<unsigned char, 1024> _buffer;

    glpp::Texture1D _text_buffer;
    Image           _font_image;

    const static std::map<char16_t, unsigned char> char_correspondance;

    Shader _shader{R"(
#version 330
out vec4 _frag_color;

in vec2 _uniform_uv;
in vec2 _raw_uv;
in vec2 _canvas_uv;

uniform sampler2D _font_image;

uniform usampler1D _text_buffer;
uniform int _sentence_size;

uniform vec4 _color;
uniform float _inflating;

void main() {

    float smoothing = 0.01;

    int letter_index = int(_raw_uv.x * float(_sentence_size));
    uint font_index = texelFetch(_text_buffer, letter_index, 0).r;

    uvec2 char_coordinates = uvec2(font_index % 16u, 15u - font_index / 16u);

    vec2 local_letter_uv = vec2(fract(_raw_uv.x * float(_sentence_size)), _raw_uv.y) / 16.;

    vec2 char_uv = vec2(char_coordinates) / 16. + local_letter_uv;

    vec4 font_texture_sample = textureGrad(_font_image, char_uv, dFdx(local_letter_uv), dFdy(local_letter_uv));
    // vec4 font_texture_sample = texture(_font_image, char_uv);

    float letter_dist_field = font_texture_sample.w - 0.5 + 1.0/256.0 - _inflating;

    _frag_color = _color * smoothstep(smoothing, -smoothing, letter_dist_field);
}
    )"};
};

} // namespace details
} // namespace p6