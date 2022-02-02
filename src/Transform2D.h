#pragma once
#include <glm/glm.hpp>
#include "Angle.h"

namespace p6 {

using Rotation = Angle;

struct Transform2D {
    glm::vec2 position{0.f};
    glm::vec2 scale{1.f};
    Rotation  rotation{0.0_radians};
};

} // namespace p6
