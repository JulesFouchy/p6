#pragma once

#include <glm/glm.hpp>
#include <glpp/glpp.hpp>
#include <string>
#include <utility>
#include <vector>

namespace p6 {

class Shader {
public:
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    explicit Shader(const std::string& fragment_source_code);

    void bind() const;
    void set(const std::string& uniform_name, int v) const;
    void set(const std::string& uniform_name, unsigned int v) const;
    void set(const std::string& uniform_name, bool v) const;
    void set(const std::string& uniform_name, float v) const;
    void set(const std::string& uniform_name, const glm::vec2& v) const;
    void set(const std::string& uniform_name, const glm::vec3& v) const;
    void set(const std::string& uniform_name, const glm::vec4& v) const;
    void set(const std::string& uniform_name, const glm::mat2& mat) const;
    void set(const std::string& uniform_name, const glm::mat3& mat) const;
    void set(const std::string& uniform_name, const glm::mat4& mat) const;

private:
    GLint uniform_location(const std::string& uniform_name) const;

private:
    glpp::UniqueProgram                                _program;
    mutable std::vector<std::pair<std::string, GLint>> _uniform_locations;
};

} // namespace p6
