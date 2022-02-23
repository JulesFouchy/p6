#include "Shader.h"
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace p6 {

static void link_program(const glpp::ext::Program& program, const glpp::VertexShader& vertex_shader, const glpp::FragmentShader& fragment_shader)
{
    program.attach_shader(*vertex_shader);
    program.attach_shader(*fragment_shader);
    program.link();
#if !defined(NDEBUG)
    const auto err = program.check_linking_errors();
    if (err) {
        throw std::runtime_error{"Shader linking failed:\n" + err.message()};
    }
#endif
}

Shader::Shader(std::string_view fragment_source_code)
{
    const auto vert = glpp::VertexShader{R"(
#version 330

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
        if (err) {
            throw std::runtime_error{"Vertex shader compilation failed:\n" + err.message()};
        }
    }
    {
        const auto err = frag.check_compilation_errors();
        if (err) {
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

Shader load_shader(std::filesystem::path fragment_shader_path)
{
    auto ifs = std::ifstream{fragment_shader_path};
    return Shader{std::string{std::istreambuf_iterator<char>{ifs}, {}}};
}

} // namespace p6
