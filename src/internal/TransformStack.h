#pragma once
#include <glm/glm.hpp>
#include <stack>
#include "../Angle.h"

namespace p6::internal {

class TransformStack {
public:
    void translate(glm::vec2);
    void rotate(p6::Angle);
    void scale(glm::vec2);
    void apply_matrix(glm::mat3 additional_transform);
    void set_matrix(glm::mat3 transform);
    void reset_matrix();

    void push_transform();
    void pop_transform();

    glm::mat3 current_matrix() const { return _current; }

private:
    glm::mat3             _current{1.f};
    std::stack<glm::mat3> _stack{};
};

} // namespace p6::internal