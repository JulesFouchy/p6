#pragma once

#include <filesystem>
#include <glpp/extended.hpp>
#include <string_view>
#include "Transform2D.h"

namespace p6
{
class Shader
{
public:
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    explicit Shader(std::string_view fragment_source_code);

    void set(std::string_view uniform_name, int value) const;
    void set(std::string_view uniform_name, unsigned int value) const;
    void set(std::string_view uniform_name, bool value) const;
    void set(std::string_view uniform_name, float value) const;
    void set(std::string_view uniform_name, const glm::vec2& value) const;
    void set(std::string_view uniform_name, const glm::vec3& value) const;
    void set(std::string_view uniform_name, const glm::vec4& value) const;
    void set(std::string_view uniform_name, const glm::mat2& value) const;
    void set(std::string_view uniform_name, const glm::mat3& value) const;
    void set(std::string_view uniform_name, const glm::mat4& value) const;

private:
    friend class Context;
    void bind() const;

private:
    glpp::ext::Program _program;
};

/// Loads a Shader from a file containing the fragment shader's source code.
/// If the path is relative, it will be relative to the directory containing your executable.
/// Throws std::runtime_error if there is an error while compiling the shader source code.
[[nodiscard]] Shader load_shader(std::filesystem::path fragment_shader_path);

namespace internal
{
/// Set all needed uniforms for the p6 default vertex shader.
void set_vertex_shader_uniforms(const Shader& shader, const Transform2D& transform, float framebuffer_aspect_ratio);

} // namespace internal

} // namespace p6
