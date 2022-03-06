#pragma once

#include <array>
#include <glpp/glpp.hpp>
#include <string>
#include "../Color.h"
#include "../Image.h"
#include "../Shader.h"

namespace p6::details {

namespace TextRendererU {
Radii compute_text_radii(const std::u16string& text, float font_size);
}

class TextRenderer {
public:
    TextRenderer();

    /// Prepares the shader to render the given text
    void          setup_rendering_for(const std::u16string& text, Color color, float inflating = 0.01f);
    const Shader& shader() const { return _shader; }

    using ArrayOfUint8 = std::array<uint8_t, 1024>;

private:
    ArrayOfUint8    _cpu_text_buffer;
    glpp::Texture1D _gpu_text_buffer;

    Image _font_image;

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

} // namespace p6::details