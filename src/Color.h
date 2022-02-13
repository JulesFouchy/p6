#pragma once

#include <glm/glm.hpp>

namespace p6 {

class Color {
public:
    Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f)
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

} // namespace p6