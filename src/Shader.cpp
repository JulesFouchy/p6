#include "Shader.h"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>

namespace p6 {

static void make_shader(const glpp::Program& program, const glpp::VertexShader& vertex_shader, const glpp::FragmentShader& fragment_shader)
{
    program.attach_shader(*vertex_shader);
    program.attach_shader(*fragment_shader);
    program.link();
    const auto err = program.check_linking_errors();
    if (err) {
        throw std::runtime_error{"Shader linking failed:\n" + err.message()};
    }
}

Shader::Shader(const std::string& fragment_source_code)
{
    static const auto vert = glpp::VertexShader{R"(
#version 330

layout(location = 0) in vec2 _vertex_position;
layout(location = 1) in vec2 _texture_coordinates;
out vec2 _uv;
out vec2 _uv_canvas_scale;

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
    _uv = _texture_coordinates;
    _uv_canvas_scale = (_texture_coordinates - 0.5) * _rect_size * 2.;
}
    )"};
    const auto        frag = glpp::FragmentShader{fragment_source_code.c_str()};
    make_shader(_program, vert, frag);
}

void Shader::bind() const
{
    _program.use();
}

GLint Shader::uniform_location(const std::string& uniform_name) const
{
    const auto it = std::find_if(_uniform_locations.begin(), _uniform_locations.end(), [&](auto&& pair) {
        return pair.first == uniform_name;
    });
    if (it != _uniform_locations.end()) {
        return it->second;
    }
    else {
        const auto location = _program.compute_uniform_location(uniform_name.c_str());
        _uniform_locations.emplace_back(uniform_name, location);
        return location;
    }
}

void Shader::set(const std::string& uniform_name, int v) const
{
    bind();
    _program.set_uniform(uniform_location(uniform_name), v);
}
void Shader::set(const std::string& uniform_name, unsigned int v) const
{
    set(uniform_name, static_cast<int>(v));
}
void Shader::set(const std::string& uniform_name, bool v) const
{
    set(uniform_name, v ? 1 : 0);
}
void Shader::set(const std::string& uniform_name, float v) const
{
    bind();
    _program.set_uniform(uniform_location(uniform_name), v);
}
void Shader::set(const std::string& uniform_name, const glm::vec2& v) const
{
    bind();
    _program.set_uniform(uniform_location(uniform_name), v.x, v.y);
}
void Shader::set(const std::string& uniform_name, const glm::vec3& v) const
{
    bind();
    _program.set_uniform(uniform_location(uniform_name), v.x, v.y, v.z);
}
void Shader::set(const std::string& uniform_name, const glm::vec4& v) const
{
    bind();
    _program.set_uniform(uniform_location(uniform_name), v.x, v.y, v.z, v.w);
}
void Shader::set(const std::string& uniform_name, const glm::mat2& mat) const
{
    bind();
    _program.set_uniform_mat2(uniform_location(uniform_name), glm::value_ptr(mat));
}
void Shader::set(const std::string& uniform_name, const glm::mat3& mat) const
{
    bind();
    _program.set_uniform_mat3(uniform_location(uniform_name), glm::value_ptr(mat));
}
void Shader::set(const std::string& uniform_name, const glm::mat4& mat) const
{
    bind();
    _program.set_uniform_mat4(uniform_location(uniform_name), glm::value_ptr(mat));
}

} // namespace p6
