#pragma once

#include <string>

namespace p6 {

struct Key {
    /// The character that was pressed. This depends on the keyboard layout (qwerty vs azerty).
    /// If the key pressed was not a character (for example ENTER, SHIFT etc.) then this is an empty string.
    std::string logical;
    /// The key at a given position on the keyboard. It is independent of the keyboard layout (e.g. GLFW_KEY_Q will correspond to the first key on the first row of your keyboard, no matter if you are using azerty instead of qwerty)
    int physical;
};

} // namespace p6
