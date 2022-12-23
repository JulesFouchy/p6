#pragma once

namespace p6::random {

/// Returns a random number between 0 and 1.
float number();

/// Returns a random number between 0 and `max`.
float number(float max);

/// Returns a random number between `min` and `max`.
float number(float min, float max);

} // namespace p6::random