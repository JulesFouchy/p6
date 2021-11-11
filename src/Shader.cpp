#include "Shader.h"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include "details/OpenGL/GLDebug.h"
#include "details/OpenGL/ShaderModule.h"

namespace p6 {

static void validate_shader(GLuint id)
{
    GLDebug(glValidateProgram(id));
    GLint result; // NOLINT
    GLDebug(glGetProgramiv(id, GL_VALIDATE_STATUS, &result));
    if (result == GL_FALSE) {
        GLsizei length;
        GLDebug(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
        std::vector<GLchar> error_message;
        error_message.reserve(length);
        GLDebug(glGetProgramInfoLog(id, length, nullptr, error_message.data()));
        throw std::runtime_error(std::string{"Linking failed:\n"} + error_message.data());
    }
}

static void make_shader(GLuint id, const details::ShaderModule& vertex_module, const details::ShaderModule& fragment_module)
{
    GLDebug(glAttachShader(id, *vertex_module));
    GLDebug(glAttachShader(id, *fragment_module));
    GLDebug(glLinkProgram(id));
    validate_shader(id);
}

Shader::Shader(const std::string& fragment_source_code)
{
    static const auto vert = details::ShaderModule{{R"(
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
                                                    details::ShaderKind::Vertex, "p6 Default Vertex Shader"}};
    const auto        frag = details::ShaderModule{{fragment_source_code, details::ShaderKind::Fragment, "User Fragment Shader"}};
    make_shader(*_shader, vert, frag);
}

void Shader::bind() const
{
    glUseProgram(*_shader);
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
        GLDebug(GLint location = glGetUniformLocation(*_shader, uniform_name.c_str()));
        std::cout << "Computing uniform location\n";
        _uniform_locations.emplace_back(uniform_name, location);
        return location;
    }
}

void Shader::set(const std::string& uniform_name, int v) const
{
    bind();
    GLDebug(glUniform1i(uniform_location(uniform_name), v));
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
    GLDebug(glUniform1f(uniform_location(uniform_name), v));
}
void Shader::set(const std::string& uniform_name, const glm::vec2& v) const
{
    bind();
    GLDebug(glUniform2f(uniform_location(uniform_name), v.x, v.y));
}
void Shader::set(const std::string& uniform_name, const glm::vec3& v) const
{
    bind();
    GLDebug(glUniform3f(uniform_location(uniform_name), v.x, v.y, v.z));
}
void Shader::set(const std::string& uniform_name, const glm::vec4& v) const
{
    bind();
    GLDebug(glUniform4f(uniform_location(uniform_name), v.x, v.y, v.z, v.w));
}
void Shader::set(const std::string& uniform_name, const glm::mat2& mat) const
{
    bind();
    GLDebug(glUniformMatrix2fv(uniform_location(uniform_name), 1, GL_FALSE, glm::value_ptr(mat)));
}
void Shader::set(const std::string& uniform_name, const glm::mat3& mat) const
{
    bind();
    GLDebug(glUniformMatrix3fv(uniform_location(uniform_name), 1, GL_FALSE, glm::value_ptr(mat)));
}
void Shader::set(const std::string& uniform_name, const glm::mat4& mat) const
{
    bind();
    GLDebug(glUniformMatrix4fv(uniform_location(uniform_name), 1, GL_FALSE, glm::value_ptr(mat)));
}

} // namespace p6
