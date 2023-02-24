#include "Shader.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include "internal/make_absolute_path.h"

namespace p6 {

GLenum Shader::s_available_texture_slot{0};

Shader::Shader(std::string_view fragment_source_code)
    : Shader{R"(
#version 410

layout(location = 0) in vec2 _vertex_position;
layout(location = 1) in vec2 _texture_coordinates;
out vec2 _raw_uv;
out vec2 _uniform_uv;
out vec2 _canvas_uv;

uniform mat3 _transform;
uniform float _window_inverse_aspect_ratio;
uniform vec2 _size;
uniform float _aspect_ratio;

void main()
{
    vec2 pos = _vertex_position;
    vec3 pos3 = _transform * vec3(pos, 1.);
    pos = pos3.xy / pos3.z;
    pos.x *= _window_inverse_aspect_ratio;
    gl_Position = vec4(pos, 0., 1.);
    _raw_uv = _texture_coordinates;
    _uniform_uv = (_texture_coordinates - 0.5) * vec2(_aspect_ratio, 1.) * 2.;
    _canvas_uv = (_texture_coordinates - 0.5) * _size * 2.;
}
    )",
             fragment_source_code}
{}

Shader::Shader(std::string_view vertex_source_code, std::string_view fragment_source_code)
{
    const auto vert = glpp::VertexShader{vertex_source_code.data()};
    const auto frag = glpp::FragmentShader{fragment_source_code.data()};
#if !defined(NDEBUG)
    {
        const auto err = vert.check_compilation_errors();
        if (err)
        {
            const auto msg = "Vertex shader compilation failed:\n" + err.message();
            std::cerr << msg << '\n';
            throw std::runtime_error{msg};
        }
    }
    {
        const auto err = frag.check_compilation_errors();
        if (err)
        {
            const auto msg = "Fragment shader compilation failed:\n" + err.message();
            std::cerr << msg << '\n';
            throw std::runtime_error{msg};
        }
    }
#endif
    _program.attach_shader(*vert);
    _program.attach_shader(*frag);
    _program.link();
#if !defined(NDEBUG)
    {
        const auto err = _program.check_linking_errors();
        if (err)
        {
            const auto msg = "Shader linking failed:\n" + err.message();
            std::cerr << msg << '\n';
            throw std::runtime_error{msg};
        }
    }
#endif
}

void Shader::check_for_errors_before_rendering() const
{
#if !defined(NDEBUG)
    const auto err = _program.check_for_state_errors();
    if (err)
    {
        const auto msg = "Shader is not ready for rendering:\n" + err.message();
        std::cerr << msg << '\n';
        throw std::runtime_error{msg};
    }
#endif
}

void Shader::use() const
{
    _program.use();
}

template<typename T>
void set_uniform(const glpp::ext::Program& program, std::string_view uniform_name, T&& value)
{
    program.use();
    program.set(std::string{uniform_name}, value);
}
void Shader::set(std::string_view uniform_name, int value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, unsigned int value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, bool value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, float value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const glm::vec2& value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const glm::vec3& value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const glm::vec4& value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const glm::mat2& value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const glm::mat3& value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const glm::mat4& value) const
{
    set_uniform(_program, uniform_name, value);
}
void Shader::set(std::string_view uniform_name, const ImageOrCanvas& image) const
{
    image.texture().bind_to_texture_unit(s_available_texture_slot);
    set_uniform(_program, uniform_name, s_available_texture_slot);
    s_available_texture_slot = (s_available_texture_slot + 1) % 8;
}

Shader load_shader(std::filesystem::path fragment_shader_path)
{
    auto ifs = std::ifstream{internal::make_absolute_path(fragment_shader_path)};
    return Shader{std::string{std::istreambuf_iterator<char>{ifs}, {}}};
}

Shader load_shader(std::filesystem::path vertex_shader_path, std::filesystem::path fragment_shader_path)
{
    auto fragment_ifs = std::ifstream{internal::make_absolute_path(fragment_shader_path)};
    auto vertex_ifs   = std::ifstream{internal::make_absolute_path(vertex_shader_path)};
    return Shader{std::string{std::istreambuf_iterator<char>{vertex_ifs}, {}},
                  std::string{std::istreambuf_iterator<char>{fragment_ifs}, {}}};
}

namespace internal {

static glm::vec2 get_scale(const glm::mat3& transform)
{
    // Get the length of the first two columns of the 2x2 sub-matrix
    return glm::vec2{
        glm::length(glm::vec2{transform[0][0], transform[0][1]}),
        glm::length(glm::vec2{transform[1][0], transform[1][1]}),
    };
}

void set_vertex_shader_uniforms(const Shader& shader, const glm::mat3& transform, float framebuffer_aspect_ratio)
{
    const glm::vec2 scale = get_scale(transform);
    shader.set("_window_aspect_ratio", framebuffer_aspect_ratio);
    shader.set("_window_inverse_aspect_ratio", 1.0f / framebuffer_aspect_ratio);
    shader.set("_transform", transform);
    shader.set("_size", scale);
    if (scale.x == 0.f || scale.y == 0.f) // Avoid crash when aspect ratio implies a division by 0
        return;
    shader.set("_aspect_ratio", scale.x / scale.y);
    shader.set("_inverse_aspect_ratio", scale.y / scale.x);
}

} // namespace internal

} // namespace p6
