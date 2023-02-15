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

glm::mat3 as_matrix(const Transform2D&);

struct Center {
    glm::vec2 value{0.f};

    Center() = default;

    Center(float x, float y)
        : value{x, y} {}

    Center(glm::vec2 value)
        : value{value} {}
};

struct TopLeftCorner {
    glm::vec2 value{0.f};

    TopLeftCorner() = default;

    TopLeftCorner(float x, float y)
        : value{x, y} {}

    TopLeftCorner(glm::vec2 value)
        : value{value} {}
};

struct TopRightCorner {
    glm::vec2 value{0.f};

    TopRightCorner() = default;

    TopRightCorner(float x, float y)
        : value{x, y} {}

    TopRightCorner(glm::vec2 value)
        : value{value} {}
};

struct BottomLeftCorner {
    glm::vec2 value{0.f};

    BottomLeftCorner() = default;

    BottomLeftCorner(float x, float y)
        : value{x, y} {}

    BottomLeftCorner(glm::vec2 value)
        : value{value} {}
};

struct BottomRightCorner {
    glm::vec2 value{0.f};

    BottomRightCorner() = default;

    BottomRightCorner(float x, float y)
        : value{x, y} {}

    BottomRightCorner(glm::vec2 value)
        : value{value} {}
};

struct Radii {
    glm::vec2 value{1.f};

    Radii() = default;

    Radii(float x, float y)
        : value{x, y} {}

    Radii(glm::vec2 v)
        : value{v} {}
};

struct Radius {
    float value{1.f};

    Radius() = default;
    Radius(float value)
        : value{value} {};
};

struct RadiusX {
    float value{1.f};

    RadiusX() = default;
    RadiusX(float value)
        : value{value} {};
};

struct RadiusY {
    float value{1.f};

    RadiusY() = default;
    RadiusY(float value)
        : value{value} {};
};

Transform2D make_transform_2D(Center, Radius, Rotation);
Transform2D make_transform_2D(Center, Radii, Rotation);
Transform2D make_transform_2D(TopLeftCorner, Radius, Rotation);
Transform2D make_transform_2D(TopLeftCorner, Radii, Rotation);
Transform2D make_transform_2D(TopRightCorner, Radius, Rotation);
Transform2D make_transform_2D(TopRightCorner, Radii, Rotation);
Transform2D make_transform_2D(BottomLeftCorner, Radius, Rotation);
Transform2D make_transform_2D(BottomLeftCorner, Radii, Rotation);
Transform2D make_transform_2D(BottomRightCorner, Radius, Rotation);
Transform2D make_transform_2D(BottomRightCorner, Radii, Rotation);

} // namespace p6
