#pragma once
#include <glm/glm.hpp>
#include "Context.h"

namespace p6::random {

/* ------------------------------- */
/** \defgroup random Random
 * Pick random numbers, points, directions, etc.
 * @{*/
/* ------------------------------- */

/// Returns a random number between 0 and 1.
float number();

/// Returns a random number between 0 and `max`.
/// Throws a `std::invalid_argument` exception if `max < 0`.
float number(float max);

/// Returns a random number between `min` and `max`.
/// Throws a `std::invalid_argument` exception if `min > max`.
float number(float min, float max);

/// Returns a point inside the main canvas of the context.
glm::vec2 point(const p6::Context&);

/// Returns a point inside the given canvas.
glm::vec2 point(const p6::Canvas&);

/// Returns a random point in a square between (-1, -1) and (1, 1).
glm::vec2 point();

/// Returns a random point in a rectangle between `min` and `max`.
/// Throws a `std::invalid_argument` exception if `min.x > max.x || min.y > max.y`.
glm::vec2 point(glm::vec2 min, glm::vec2 max);

/// Returns a random point in a rectangle between (-aspect_ratio, -1) and (aspect_ratio, 1).
glm::vec2 point(float aspect_ratio);

/// Returns a random direction (vector of length 1).
glm::vec2 direction();

/// Returns a random angle.
Angle angle();

/**@}*/
} // namespace p6::random