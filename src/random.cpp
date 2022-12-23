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

    const auto distribution = std::uniform_real_distribution<float>{min, max};
    return distribution(generator());
}

} // namespace p6::random