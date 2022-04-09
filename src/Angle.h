#pragma once

#include <op/op.hpp>
#include "math_constants.h"

namespace p6
{
struct Radians
    : public op::Addable<Radians>
    , public op::Subtractable<Radians>
    , public op::Negatable<Radians>
    , public op::Scalable<Radians>
{
    float value{0.f};

    constexpr Radians() = default;

    constexpr explicit Radians(float value)
        : value{value} {}
};

constexpr Radians turns_to_radians(float turns)
{
    return Radians{turns * TAU};
}

constexpr Radians degrees_to_radians(float degrees)
{
    return Radians{degrees / 360.f * TAU};
}

inline float radians_to_degrees(Radians radians)
{
    return radians.value / TAU * 360.f;
}

inline float radians_to_turns(Radians radians)
{
    return radians.value / TAU;
}

class Angle
    : public op::Addable<Angle>
    , public op::Subtractable<Angle>
    , public op::Negatable<Angle>
    , public op::Scalable<Angle>
{
public:
    constexpr Angle() = default;
    constexpr explicit Angle(Radians value)
        : value{value} {}

    float as_turns() const { return radians_to_turns(value); }
    float as_radians() const { return value.value; }
    float as_degrees() const { return radians_to_degrees(value); }

    Radians value{};

private:
    // Radians value;
};

} // namespace p6

constexpr p6::Angle operator""_turn(long double turns)
{
    return p6::Angle{p6::turns_to_radians(static_cast<float>(turns))};
}

constexpr p6::Angle operator""_degrees(long double degrees)
{
    return p6::Angle{p6::degrees_to_radians(static_cast<float>(degrees))};
}

constexpr p6::Angle operator""_radians(long double radians)
{
    return p6::Angle{p6::Radians{static_cast<float>(radians)}};
}

constexpr p6::Angle operator""_turn(unsigned long long int turns)
{
    return operator""_turn(static_cast<long double>(turns));
}

constexpr p6::Angle operator""_degrees(unsigned long long int degrees)
{
    return operator""_degrees(static_cast<long double>(degrees));
}

constexpr p6::Angle operator""_radians(unsigned long long int radians)
{
    return operator""_radians(static_cast<long double>(radians));
}
