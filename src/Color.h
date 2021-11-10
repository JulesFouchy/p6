#pragma once

namespace p6 {

class Color {
public:
    Color(float r, float g, float b, float a = 1.f)
        : _r{r}, _g{g}, _b{b}, _a{a}
    {
    }

    float r() const { return _r; }
    float g() const { return _g; }
    float b() const { return _b; }
    float a() const { return _a; }

private:
    float _r;
    float _g;
    float _b;
    float _a;
};

} // namespace p6