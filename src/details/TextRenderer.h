#pragma once

#include "RectRenderer.h"
#include "..\Shader.h"
#include "..\Image.h"
#include "..\Color.h"

#include <glpp/glpp.hpp>
#include <array>
#include <string>
#include <map>

namespace p6 {
namespace details {

class TextRenderer {
public:
    TextRenderer();

    void render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float inflating, Transform2D transform, Color = {0.0f, 0.0f, 0.0f});
    
    void render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float font_size, float inflating, Center, Rotation = {}, Color = {0.0f, 0.0f, 0.0f});
    void render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float font_size, float inflating, TopLeftCorner, Rotation = {}, Color = {0.0f, 0.0f, 0.0f});
    void render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float font_size, float inflating, TopRightCorner, Rotation = {}, Color = {0.0f, 0.0f, 0.0f});
    void render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float font_size, float inflating, BottomLeftCorner, Rotation = {}, Color = {0.0f, 0.0f, 0.0f});
    void render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float font_size, float inflating, BottomRightCorner, Rotation = {}, Color = {0.0f, 0.0f, 0.0f});

private:
    void Update_buffer_from_str(const std::u16string& str);
    void Update_data(const std::u16string& str);
    static size_t Compute_sentence_size(const std::u16string& str);
    
    std::array<unsigned char, 1024> _buffer;

    glpp::Texture1D _textBuffer;
    Image _fontImage;

    const static std::map<char16_t, unsigned char> char_correspondance;

    Shader                            _shader{R"(
#version 330
out vec4 _frag_color;

in vec2 _uniform_uv;
in vec2 _raw_uv;
in vec2 _canvas_uv;

uniform sampler2D _fontImage;

uniform usampler1D _textBuffer;
uniform int _sentence_size;

uniform vec3 _color;
uniform float _inflating;

void main() {

    float smoothing = 0.01;

    int letter_index = int(_raw_uv.x * float(_sentence_size));
    uint font_index = texelFetch(_textBuffer, letter_index, 0).r;

    uvec2 char_coordinates = uvec2(font_index % 16u, 15u - font_index / 16u);

    vec2 local_letter_uv = vec2(fract(_raw_uv.x * float(_sentence_size)), _raw_uv.y) / 16.;

    vec2 char_uv = vec2(char_coordinates) / 16. + local_letter_uv;

    vec4 font_texture_sample = textureGrad(_fontImage, char_uv, dFdx(local_letter_uv), dFdy(local_letter_uv));
    // vec4 font_texture_sample = texture(_fontImage, char_uv);

    float letterDistField = font_texture_sample.w - 0.5 + 1.0/256.0 - _inflating;

    _frag_color.rgba = vec4(_color, smoothstep(smoothing, -smoothing, letterDistField));
}
    )"};

};

namespace internal {
    Center compute_new_center(glm::vec2 offset_to_center, glm::vec2 corner_position, Radii radii, Rotation rotation);
} // namespace internal

} // namespace details
} // namespace p6