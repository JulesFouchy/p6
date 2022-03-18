#pragma once

#include <glm/glm.hpp>
#include <string_view>
#include <variant>

namespace p6 {

class Color {
public:
    constexpr Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f)
        : _r{r}, _g{g}, _b{b}, _a{a}
    {
    }

    float     r() const { return _r; }
    float     g() const { return _g; }
    float     b() const { return _b; }
    float     a() const { return _a; }
    glm::vec3 as_premultiplied_vec3() const { return as_straight_vec3() * _a; }
    glm::vec3 as_straight_vec3() const { return {_r, _g, _b}; }
    glm::vec4 as_premultiplied_vec4() const { return {as_premultiplied_vec3(), _a}; }
    glm::vec4 as_straight_vec4() const { return {_r, _g, _b, _a}; }

private:
    float _r;
    float _g;
    float _b;
    float _a;
};

constexpr Color rgb(glm::vec3 rgb_values)
{
    return Color(rgb_values.r, rgb_values.g, rgb_values.b);
}

constexpr Color rgb(float r, float g, float b)
{
    return rgb(glm::vec3{r, g, b});
}

constexpr Color hex(std::string_view)
{
    return rgb(1.f, 1.f, 0.f);
}

constexpr Color hex(unsigned int hex_code)
{
    assert(hex_code <= 0xFFFFFF);
    return Color(static_cast<float>((hex_code & 0xFF0000u) >> 16) / 255.f,
                 static_cast<float>((hex_code & 0x00FF00u) >> 8) / 255.f,
                 static_cast<float>((hex_code & 0x0000FFu) >> 0) / 255.f);
}

class BlendMode_Overwrite {
};
class BlendMode_Blend {
};
class BlendMode_Add {
};

using BlendMode = std::variant<BlendMode_Overwrite,
                               BlendMode_Blend,
                               BlendMode_Add>;

struct Material {
    Color     color;
    BlendMode blend_mode;
};

} // namespace p6