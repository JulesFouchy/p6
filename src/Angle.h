#pragma once

#include <op/op.hpp>
#include "math.h"

namespace p6 {

struct Radians
    : public op::Addable<Radians>
    , public op::Subtractable<Radians>
    , public op::Negatable<Radians>
    , public op::Scalable<Radians> {
    float value{0.f};

    Radians() = default;

    explicit Radians(float value)
        : value{value} {}
};

inline Radians turns_to_radians(float turns)
{
    return Radians{turns * TAU};
}

inline Radians degrees_to_radians(float degrees)
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
    , public op::Scalable<Angle> {
public:
    Angle() = default;
    explicit Angle(Radians value)
        : value{value} {}

    float as_turns() const { return radians_to_turns(value); }
    float as_radians() const { return value.value; }
    float as_degrees() const { return radians_to_degrees(value); }

    Radians value{};

private:
    // Radians value;
};

} // namespace p6

inline p6::Angle operator"" _turns(long double turns)
{
    return p6::Angle{p6::turns_to_radians(static_cast<float>(turns))};
}

inline p6::Angle operator"" _degrees(long double degrees)
{
    return p6::Angle{p6::degrees_to_radians(static_cast<float>(degrees))};
}

inline p6::Angle operator"" _radians(long double radians)
{
    return p6::Angle{p6::Radians{static_cast<float>(radians)}};
}