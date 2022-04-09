#include "math.h"
#include <glm/gtx/rotate_vector.hpp>

namespace p6
{
glm::vec2 rotated_by(Angle angle, glm::vec2 vector)
{
    return glm::rotate(vector, angle.as_radians());
}

} // namespace p6
