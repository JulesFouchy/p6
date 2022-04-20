#include "TriangleRenderer.h"
#include <array>
#include <stdexcept>

namespace p6::details
{
TriangleRenderer::TriangleRenderer()
{
    const auto vertex_shader = glpp::VertexShader{R"(
#version 330

uniform vec2 _p1;
uniform vec2 _p2;
uniform vec2 _p3;
uniform float _window_aspect_ratio;

void main()
{
    if (gl_VertexID == 0) gl_Position = vec4(_p1 * vec2(1./_window_aspect_ratio, 1.), 0., 1.);
    if (gl_VertexID == 1) gl_Position = vec4(_p2 * vec2(1./_window_aspect_ratio, 1.), 0., 1.);
    if (gl_VertexID == 2) gl_Position = vec4(_p3 * vec2(1./_window_aspect_ratio, 1.), 0., 1.);
}
    )"};

    const auto fragment_shader = glpp::FragmentShader{R"(
#version 330
out vec4 _frag_color;

uniform vec2 _p1;
uniform vec2 _p2;
uniform vec2 _p3;
uniform float _window_height;
uniform float _window_aspect_ratio;
uniform vec4 _fill_material;
uniform vec4 _stroke_material;
uniform float _stroke_weight;

float dist(vec2 uv, vec2 p1, vec2 p2)
{
    vec2 dir12 = normalize(p2 - p1);
    vec2 uv1 = uv - p1;
    return length(dir12 * dot(dir12, uv1) - uv1);
}

void main()
{
    vec2 uv = 2. * (gl_FragCoord.xy / _window_height - vec2(0.5 * _window_aspect_ratio, 0.5));
    float d12 = dist(uv, _p1, _p2);
    float d = min(min(dist(uv, _p1, _p2), dist(uv, _p1, _p3)), dist(uv, _p2, _p3));
    _frag_color = (d < _stroke_weight) 
                    ? _stroke_material
                    : _fill_material;
}
    )"};

#if !defined(NDEBUG)
    {
        const auto err = vertex_shader.check_compilation_errors();
        if (err)
        {
            throw std::runtime_error{"Vertex shader compilation failed:\n" + err.message()};
        }
    }
    {
        const auto err = fragment_shader.check_compilation_errors();
        if (err)
        {
            throw std::runtime_error{"Fragment shader compilation failed:\n" + err.message()};
        }
    }
#endif
    _shader.attach_shader(*vertex_shader);
    _shader.attach_shader(*fragment_shader);
    _shader.link();
}

void TriangleRenderer::render(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3,
                              float framebuffer_height, float framebuffer_ratio,
                              const std::optional<glm::vec4>& fill_material,
                              const std::optional<glm::vec4>& stroke_material,
                              float                           stroke_weight) const
{
    if (!fill_material && !stroke_material)
        return;
    _shader.use();
    _shader.set("_p1", p1);
    _shader.set("_p2", p2);
    _shader.set("_p3", p3);
    _shader.set("_window_height", framebuffer_height);
    _shader.set("_window_aspect_ratio", framebuffer_ratio);
    _shader.set("_fill_material", fill_material.value_or(glm::vec4{0.f}));
    _shader.set("_stroke_material", stroke_material ? *stroke_material : *fill_material);
    _shader.set("_stroke_weight", stroke_weight);
    glBindVertexArray(*_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

} // namespace p6::details