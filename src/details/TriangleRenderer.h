#pragma once

#include <glm/glm.hpp>
#include <glpp/extended.hpp>
#include <optional>

namespace p6::details {

class TriangleRenderer {
public:
    TriangleRenderer();
    void render(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3,
                float framebuffer_height, float framebuffer_ratio,
                const std::optional<glm::vec4>& fill_material,
                const std::optional<glm::vec4>& stroke_material,
                float                           stroke_weight) const;

private:
    glpp::UniqueVertexArray _vao;
    glpp::ext::Program      _shader;
};

} // namespace p6::details