#pragma once

#include <glm/glm.hpp>

namespace p6 {

enum class Button {
    Left,
    Right,
    Middle,
};

struct MouseButton {
    glm::vec2 position;
    Button    button;
};

} // namespace p6
