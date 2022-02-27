#include "Transform2D.h"

#include "math.h"

namespace p6 {

Transform2D make_transform_2D(Center center, Radius radius, Rotation rotation)
{
    return make_transform_2D(center,
                             Radii{radius.value, radius.value},
                             rotation);
}

Transform2D make_transform_2D(Center center, Radii radii, Rotation rotation)
{
    return {center.value,
            radii.value,
            rotation};
}

Transform2D make_transform_2D_impl(glm::vec2 offset_to_center, glm::vec2 corner_position, Radii radii, Rotation rotation)
{
    return make_transform_2D(Center{corner_position + rotated_by(rotation, radii.value * offset_to_center)},
                             radii,
                             rotation);
}

Transform2D make_transform_2D(TopLeftCorner corner, Radius radius, Rotation rotation)
{
    return make_transform_2D(corner, Radii{radius.value, radius.value}, rotation);
}

Transform2D make_transform_2D(TopLeftCorner corner, Radii radii, Rotation rotation)
{
    return make_transform_2D_impl({1, -1}, corner.value, radii, rotation);
}

Transform2D make_transform_2D(TopRightCorner corner, Radius radius, Rotation rotation)
{
    return make_transform_2D(corner, Radii{radius.value, radius.value}, rotation);
}

Transform2D make_transform_2D(TopRightCorner corner, Radii radii, Rotation rotation)
{
    return make_transform_2D_impl({-1, -1}, corner.value, radii, rotation);
}

Transform2D make_transform_2D(BottomLeftCorner corner, Radius radius, Rotation rotation)
{
    return make_transform_2D(corner, Radii{radius.value, radius.value}, rotation);
}

Transform2D make_transform_2D(BottomLeftCorner corner, Radii radii, Rotation rotation)
{
    return make_transform_2D_impl({1, 1}, corner.value, radii, rotation);
}

Transform2D make_transform_2D(BottomRightCorner corner, Radius radius, Rotation rotation)
{
    return make_transform_2D(corner, Radii{radius.value, radius.value}, rotation);
}

Transform2D make_transform_2D(BottomRightCorner corner, Radii radii, Rotation rotation)
{
    return make_transform_2D_impl({-1, 1}, corner.value, radii, rotation);
}

} // namespace p6