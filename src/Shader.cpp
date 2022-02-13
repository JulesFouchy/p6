#include "Shader.h"
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

Shader::Shader(const std::string& fragment_source_code)
{
    static const auto vert = glpp::VertexShader{R"(
#version 330

layout(location = 0) in vec2 _vertex_position;
layout(location = 1) in vec2 _texture_coordinates;
out vec2 _raw_uv;
out vec2 _uniform_uv;

uniform mat3 _transform;
uniform float _inverse_aspect_ratio;
uniform vec2 _rect_size;

void main()
{
    vec2 pos = _vertex_position;
    vec3 pos3 = _transform * vec3(pos, 1.);
    pos = pos3.xy / pos3.z;
    pos.x *= _inverse_aspect_ratio;
    gl_Position = vec4(pos, 0., 1.);
    _raw_uv = _texture_coordinates;
    _uniform_uv = (_texture_coordinates - 0.5) * _rect_size * 2.;
}
    )"};
    const auto        frag = glpp::FragmentShader{fragment_source_code.c_str()};
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
void set_uniform(const glpp::ext::Program& program, const std::string& uniform_name, T&& v)
{
    program.use();
    program.set(uniform_name, v);
}
void Shader::set(const std::string& uniform_name, int v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, unsigned int v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, bool v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, float v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, const glm::vec2& v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, const glm::vec3& v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, const glm::vec4& v) const
{
    set_uniform(_program, uniform_name, v);
}
void Shader::set(const std::string& uniform_name, const glm::mat2& mat) const
{
    set_uniform(_program, uniform_name, mat);
}
void Shader::set(const std::string& uniform_name, const glm::mat3& mat) const
{
    set_uniform(_program, uniform_name, mat);
}
void Shader::set(const std::string& uniform_name, const glm::mat4& mat) const
{
    set_uniform(_program, uniform_name, mat);
}

} // namespace p6
