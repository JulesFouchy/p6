#pragma once

namespace p6::random {

/// Returns a random number between 0 and 1.
float number();

/// Returns a random number between 0 and `max`.
/// Throws a `std::invalid_argument` exception if `max < 0`.
float number(float max);

/// Returns a random number between `min` and `max`.
/// Throws a `std::invalid_argument` exception if `min > max`.
float number(float min, float max);

} // namespace p6::random