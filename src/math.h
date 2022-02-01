#pragma once

namespace p6 {
/* ------------------------------- */
/** \defgroup math Math
 * Mathematical functions and numerical constants.
 * @{*/
/* ------------------------------- */

/// Maps x, which is in the range [from_mix, from_max], to the range [to_min, to_max].
/// For example if x == from_min, then the function returns to_min
/// if x == (from_min + from_max) / 2 then the function returns (to_min + to_max) / 2
/// if x is outside of the range [from_min, from_max] the result will be outside of [to_min, to_max], the same linear behaviour still applies
template<typename T>
T map(T x, T from_min, T from_max, T to_min, T to_max)
{
    return (x - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

static constexpr float TAU = 6.2831853f;
static constexpr float PI  = TAU / 2.f;

/**@}*/
} // namespace p6
