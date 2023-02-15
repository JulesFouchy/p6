#include "random.h"
#include <random>
#include <stdexcept>

namespace p6::random {

static auto& generator()
{
    thread_local std::default_random_engine gen{std::random_device{}()};
    return gen;
}

float number()
{
    return number(0.f, 1.f);
}

float number(float max)
{
    return number(0.f, max);
}

float number(float min, float max)
{
    if (min > max)
        throw std::invalid_argument{"`min` must be smaller than `max`"};

    auto distribution = std::uniform_real_distribution<float>{min, max};
    return distribution(generator());
}

int integer(int max)
{
    return integer(0, max);
}

int integer(int min, int max)
{
    if (min >= max)
        throw std::invalid_argument{"`min` must be strictly smaller than `max`"};

    auto distribution = std::uniform_int_distribution<int>{min, max - 1};
    return distribution(generator());
}

glm::vec2 point(const p6::Context& ctx)
{
    return point(ctx.main_canvas());
}

glm::vec2 point(const p6::Canvas& canvas)
{
    return point(canvas.aspect_ratio());
}

glm::vec2 point()
{
    return point(
        {-1.f, -1.f},
        {1.f, 1.f});
}

glm::vec2 point(glm::vec2 min, glm::vec2 max)
{
    return glm::vec2{
        number(min.x, max.x),
        number(min.y, max.y),
    };
}

glm::vec2 point(float aspect_ratio)
{
    return point(
        {-aspect_ratio, -1.f},
        {aspect_ratio, 1.f});
}

glm::vec2 direction()
{
    const auto agl = angle();
    return glm::vec2{
        std::cos(agl.as_radians()),
        std::sin(agl.as_radians()),
    };
}

Angle angle()
{
    return Angle{Radians(number(TAU))};
}

} // namespace p6::random