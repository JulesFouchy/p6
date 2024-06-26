#include "TransformStack.h"
#include <stdexcept>
#include "glm/fwd.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"

namespace p6::internal {

void TransformStack::translate(glm::vec2 translation)
{
    apply_matrix(glm::translate(glm::mat3{1.f}, translation));
}

void TransformStack::rotate(p6::Angle rotation)
{
    apply_matrix(glm::rotate(glm::mat3{1.f}, rotation.as_radians()));
}

void TransformStack::scale(glm::vec2 scale_factor)
{
    apply_matrix(glm::scale(glm::mat3{1.f}, scale_factor));
}

void TransformStack::apply_matrix(glm::mat3 additional_transform)
{
    _current *= additional_transform;
}

void TransformStack::set_matrix(glm::mat3 transform)
{
    _current = transform;
}

void TransformStack::reset_matrix()
{
    _current = glm::mat3{1.f};
}

void TransformStack::push_transform()
{
    _stack.push(_current);
}

void TransformStack::pop_transform()
{
    if (_stack.empty())
        throw std::runtime_error{"[p6 error] Called pop_transform() without a matching push_transform() before it."};
    _current = _stack.top();
    _stack.pop();
}

} // namespace p6::internal