#include "Shader.h"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>

namespace p6 {

static void validate_shader(GLuint id)
{
    glValidateProgram(id);
    glpp::check_errors();
    GLint result; // NOLINT
    glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
    glpp::check_errors();
    if (result == GL_FALSE) {
        GLsizei length;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
        glpp::check_errors();
        std::vector<GLchar> error_message;
        error_message.reserve(static_cast<size_t>(length));
        glGetProgramInfoLog(id, length, nullptr, error_message.data());
        glpp::check_errors();
        throw std::runtime_error(std::string{"Linking failed:\n"} + error_message.data());
    }
}

static void make_shader(GLuint id, const glpp::ShaderModule& vertex_module, const glpp::ShaderModule& fragment_module)
{
    glAttachShader(id, *vertex_module);
    glpp::check_errors();
    glAttachShader(id, *fragment_module);
    glpp::check_errors();
    glLinkProgram(id);
    glpp::check_errors();
    validate_shader(id);
}

Shader::Shader(const std::string& fragment_source_code)
{
    static const auto vert = glpp::ShaderModule{{R"(
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
    )",
                                                 glpp::ShaderKind::Vertex, "p6 Default Vertex Shader"}};
    const auto        frag = glpp::ShaderModule{{fragment_source_code, glpp::ShaderKind::Fragment, "User Fragment Shader"}};
    make_shader(*_shader, vert, frag);
}

void Shader::bind() const
{
    glUseProgram(*_shader);
    glpp::check_errors();
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
        GLint location = glGetUniformLocation(*_shader, uniform_name.c_str());
        glpp::check_errors();
        _uniform_locations.emplace_back(uniform_name, location);
        return location;
    }
}

void Shader::set(const std::string& uniform_name, int v) const
{
    bind();
    glUniform1i(uniform_location(uniform_name), v);
    glpp::check_errors();
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
    glUniform1f(uniform_location(uniform_name), v);
    glpp::check_errors();
}
void Shader::set(const std::string& uniform_name, const glm::vec2& v) const
{
    bind();
    glUniform2f(uniform_location(uniform_name), v.x, v.y);
    glpp::check_errors();
}
void Shader::set(const std::string& uniform_name, const glm::vec3& v) const
{
    bind();
    glUniform3f(uniform_location(uniform_name), v.x, v.y, v.z);
    glpp::check_errors();
}
void Shader::set(const std::string& uniform_name, const glm::vec4& v) const
{
    bind();
    glUniform4f(uniform_location(uniform_name), v.x, v.y, v.z, v.w);
    glpp::check_errors();
}
void Shader::set(const std::string& uniform_name, const glm::mat2& mat) const
{
    bind();
    glUniformMatrix2fv(uniform_location(uniform_name), 1, GL_FALSE, glm::value_ptr(mat));
    glpp::check_errors();
}
void Shader::set(const std::string& uniform_name, const glm::mat3& mat) const
{
    bind();
    glUniformMatrix3fv(uniform_location(uniform_name), 1, GL_FALSE, glm::value_ptr(mat));
    glpp::check_errors();
}
void Shader::set(const std::string& uniform_name, const glm::mat4& mat) const
{
    bind();
    glUniformMatrix4fv(uniform_location(uniform_name), 1, GL_FALSE, glm::value_ptr(mat));
    glpp::check_errors();
}

} // namespace p6
