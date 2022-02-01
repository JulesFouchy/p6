#pragma once
#include <glm/glm.hpp>
#include "Angle.h"

namespace p6 {

struct Transform2D {
    glm::vec2 position{0.f};
    glm::vec2 scale{1.f};
    Angle     rotation{0.0_radians};
};

} // namespace p6
