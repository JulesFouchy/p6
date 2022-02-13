#pragma once

#include <filesystem>
#include <glpp/extended.hpp>

namespace p6 {

class Shader {
public:
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    explicit Shader(const std::string& fragment_source_code);

    void set(const std::string& uniform_name, int value) const;
    void set(const std::string& uniform_name, unsigned int value) const;
    void set(const std::string& uniform_name, bool value) const;
    void set(const std::string& uniform_name, float value) const;
    void set(const std::string& uniform_name, const glm::vec2& value) const;
    void set(const std::string& uniform_name, const glm::vec3& value) const;
    void set(const std::string& uniform_name, const glm::vec4& value) const;
    void set(const std::string& uniform_name, const glm::mat2& value) const;
    void set(const std::string& uniform_name, const glm::mat3& value) const;
    void set(const std::string& uniform_name, const glm::mat4& value) const;

private:
    friend class Context;
    void bind() const;

private:
    glpp::ext::Program _program;
};

/// Loads a Shader from a file containing the fragment shader's source code
/// Throws std::runtime_error if there is an error while compiling the shader source code
Shader load_shader(std::filesystem::path fragment_shader_path);

} // namespace p6
