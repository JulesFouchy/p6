#pragma once
#include <glm/glm.hpp>
#include <string>
#include <variant>

namespace p6 {

struct MouseMove {
    glm::vec2 position;
    glm::vec2 delta;
};

struct MouseDrag {
    glm::vec2 position;
    glm::vec2 delta;
    glm::vec2 start_position;
};

struct MouseScroll {
    float dx;
    float dy;
};

enum class Button {
    Left,
    Right,
    Middle,
};

struct MouseButton {
    glm::vec2 position;
    Button    button;
};

struct Key {
    /// The character that was pressed. This depends on the keyboard layout (qwerty vs azerty).
    /// If the key pressed was not a character (for example ENTER, SHIFT etc.) then this is an empty string.
    std::string logical;
    /// The key at a given position on the keyboard. It is independent of the keyboard layout (e.g. GLFW_KEY_Q will correspond to the first key on the first row of your keyboard, no matter if you are using azerty instead of qwerty)
    int physical;
};

struct Event_Update {};
struct Event_MouseMoved {
    MouseMove data;
};
struct Event_MouseDragged {
    MouseDrag data;
};
struct Event_MousePressed {
    MouseButton data;
};
struct Event_MouseReleased {
    MouseButton data;
};
struct Event_MouseScrolled {
    MouseScroll data;
};
struct Event_KeyPressed {
    Key data;
};
struct Event_KeyReleased {
    Key data;
};
struct Event_KeyRepeated {
    Key data;
};
struct Event_MainCanvasResized {};

using Event = std::variant<
    Event_Update,
    Event_MouseMoved,
    Event_MouseDragged,
    Event_MousePressed,
    Event_MouseReleased,
    Event_MouseScrolled,
    Event_KeyPressed,
    Event_KeyReleased,
    Event_KeyRepeated,
    Event_MainCanvasResized>;

} // namespace p6