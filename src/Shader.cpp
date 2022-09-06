#include "Shader.h"
#include <fstream>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <iterator>
#include <stdexcept>
#include "internal/make_absolute_path.h"

namespace p6 {

GLenum Shader::s_available_texture_slot{0};

static void link_program(const glpp::ext::Program& program, const glpp::VertexShader& vertex_shader, const glpp::FragmentShader& fragment_shader)
{
    program.attach_shader(*vertex_shader);
    program.attach_shader(*fragment_shader);
    program.link();
}

Shader::Shader(std::string_view fragment_source_code)
{
    const auto vert = glpp::VertexShader{R"(
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
    )"};
    const auto frag = glpp::FragmentShader{fragment_source_code.data()};
#if !defined(NDEBUG)
    {
        const auto err = vert.check_compilation_errors();
        if (err)
        {
            throw std::runtime_error{"Vertex shader compilation failed:\n" + err.message()};
        }
    }
    {
        const auto err = frag.check_compilation_errors();
        if (err)
        {
            throw std::runtime_error{"Fragment shader compilation failed:\n" + err.message()};
        }
    }
#endif
    link_program(_program, vert, frag);
}

void Shader::bind() const
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

namespace internal {

void set_vertex_shader_uniforms(const Shader& shader, const Transform2D& transform, float framebuffer_aspect_ratio)
{
    shader.set("_window_aspect_ratio", framebuffer_aspect_ratio);
    shader.set("_window_inverse_aspect_ratio", 1.0f / framebuffer_aspect_ratio);
    shader.set("_transform", glm::scale(glm::rotate(glm::translate(glm::mat3{1.f},
                                                                   transform.position),
                                                    transform.rotation.as_radians()),
                                        transform.scale));
    shader.set("_size", transform.scale);
    shader.set("_aspect_ratio", transform.scale.x / transform.scale.y);
    shader.set("_inverse_aspect_ratio", transform.scale.y / transform.scale.x);
}

} // namespace internal

} // namespace p6
