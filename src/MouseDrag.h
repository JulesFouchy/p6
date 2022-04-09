#pragma once

#include <glm/glm.hpp>

namespace p6
{
struct MouseDrag
{
    glm::vec2 position;
    glm::vec2 delta;
    glm::vec2 start_position;
};

} // namespace p6
