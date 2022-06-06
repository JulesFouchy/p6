#pragma once

#include <glm/glm.hpp>
#include "Angle.h"

namespace p6 {

/* ------------------------------- */
/** \defgroup math Math
 * Mathematical functions and numerical constants.
 * @{*/
/* ------------------------------- */

/// Maps x, which is in the range `[from_min, from_max]` to the range `[to_min, to_max]`.
/// For example if `x == from_min` then the function returns `to_min`.
///
/// If `x == (from_min + from_max) / 2` then the function returns `(to_min + to_max) / 2`.
///
/// If x is outside of the range `[from_min, from_max]` the result will be outside of `[to_min, to_max]`, the same linear behaviour still applies.
template<typename T>
T map(T x, T from_min, T from_max, T to_min, T to_max)
{
    return (x - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

/// Returns a new vector equal to `vector` rotated by `angle`
glm::vec2 rotated_by(Angle angle, glm::vec2 vector);

/**@}*/
} // namespace p6
