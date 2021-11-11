#pragma once

#include <glm/glm.hpp>

namespace p6 {

struct RectangleParams {
    glm::vec2 position = glm::vec2{0.f};
    glm::vec2 size     = glm::vec2{1.f};
    float     rotation = 0.f;
};

} // namespace p6
